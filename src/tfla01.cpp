/*
 * Copyright (c) 2005-2009, Bernhard Walle
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; You may only use
 * version 2 of the License, you have no option to use any other version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * -------------------------------------------------------------------------------------------------
 */
#include <memory>

#include <QLocale>
#include <QTimer>
#include <QEventLoop>
#include <QCursor>
#include <QIcon>
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QColorDialog>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QToolBar>

#include "tfla01.h"
#include "settings.h"
#include "datacollector.h"
#include "aboutdialog.h"
#include "tfla01config.h"
#include "platformhelpers.h"
#include "protocolsettingsdialog.h"
#include "protocol/analyzermanager.h"
#if HAVE_LIBIEEE1284
#  include "hardware/parportlist.h"
#endif

// -------------------------------------------------------------------------------------------------
Tfla01 *Tfla01::m_instance = NULL;

// -------------------------------------------------------------------------------------------------
Tfla01::Tfla01()
    throw ()
  : m_portsMenu(NULL)
{
    // Title and Icon
    setWindowIcon(QPixmap(":/tfla-01_32.png"));
    setWindowTitle(tr("The Fabulous Logic Analyzer"));

    setIconSize(QSize(24, 24));

    // main widget in the center TODO
    m_centralWidget = new CentralWidget(this);
    setCentralWidget(m_centralWidget);

    // Initiamlization of menu
    initActions();
    initMenubar();
    initToolbar();

    // restore the layout
    if (Settings::set().readBoolEntry("Main Window/maximized"))
        showMaximized();
    else {
        int width = int(qApp->desktop()->width() * 0.5);
        resize(
            Settings::set().readNumEntry("Main Window/width", width),
            Settings::set().readNumEntry("Main Window/height", width*3/4)
        );
    }

    // disable some actions
    m_actions.stopAction->setEnabled(false);
    updateProtocolEnabledStatus();

    connectSignalsAndSlots();
    statusBar()->setSizeGripEnabled(true);
    setUnifiedTitleAndToolBarOnMac(true);

    // don't show icons in the menu bar by default since that seems to be rather seldom
    // on the Mac platform
    if (RUNNING_ON_MAC) {
        qApp->setAttribute(Qt::AA_DontShowIconsInMenus);
    }

    m_updateChecker = new UpdateChecker(this);
    QTimer::singleShot(0, m_updateChecker, SLOT(checkForUpdate()));
}

// -------------------------------------------------------------------------------------------------
Tfla01 *Tfla01::instance()
{
    if (!m_instance)
        m_instance = new Tfla01;
    return m_instance;
}

// -------------------------------------------------------------------------------------------------
void Tfla01::portChange(int portNumber)
    throw ()
{
#if HAVE_LIBIEEE1284
    Settings::set().writeEntry("Hardware/Parport", portNumber);

    ParportList* list = ParportList::instance(0);
    for (int i = 0; i < list->getNumberOfPorts(); i++) {
        QAction *action = m_actions.portActions[i];
        action->setChecked(i == portNumber);
    }
#endif
}

// -------------------------------------------------------------------------------------------------
void Tfla01::updateProtocolEnabledStatus()
    throw ()
{
    QList<QAction *> analyzeActions = m_actions.analyzeActions->actions();
    for (int i = 0; i < analyzeActions.size(); ++i) {
        QAction *action = analyzeActions[i];
        ProtocolAnalyzer *analzyer = AnalyzerManager::instance().getAnalyzerById(action->data().toString());
        action->setEnabled(analzyer->readChannelSetting());
    }
}

// -------------------------------------------------------------------------------------------------
void Tfla01::portChanged(QAction *action)
    throw ()
{
    portChange(action->data().toInt());
}

// -------------------------------------------------------------------------------------------------
void Tfla01::startAnalyze()
    throw ()
{
    QLocale loc;

    m_actions.startAction->setEnabled(false);
    m_actions.stopAction->setEnabled(true);
    m_analyzingActive = true;

    Settings& set = Settings::set();

    std::unique_ptr<DataCollector> coll(new DataCollector(set.readNumEntry("Hardware/Parport")));
    coll->setCollectingTime(  (  set.readNumEntry("Measuring/Triggering/Minutes") * 60 +
                                 set.readNumEntry("Measuring/Triggering/Seconds") ) * 1000  );
    coll->setTriggering(true, set.readNumEntry("Measuring/Triggering/Value"),
                              set.readNumEntry("Measuring/Triggering/Mask") );
    coll->setNumberOfSkips(set.readNumEntry("Measuring/Number_Of_Skips"));

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    coll->start();

    // loop while the thread has finished
    QTimer timer;
    timer.setSingleShot(false);
    timer.start(100);
    while (coll->isRunning() && m_analyzingActive)
        qApp->processEvents(QEventLoop::WaitForMoreEvents);
    timer.stop();

    // stopped by the user
    if (!m_analyzingActive) {
        coll->stop();
        coll->wait();
    }
    QApplication::restoreOverrideCursor();

    // finished
    if (!coll->getErrorString().isNull()) {
        QMessageBox::warning(this, tr("TFLA-01"),
            tr("<qt><nobr>An error occurred while collecting data:</nobr><br>%1</qt>").arg(
                coll->getErrorString()),
            QMessageBox::Ok, QMessageBox::NoButton);
        goto end;
    }

    statusBar()->showMessage(tr("Collected %1 samples successfully.").arg(
                         loc.toString( double(coll->getData().bytes().size()), 'g',
                                       QString::number(coll->getData().bytes().size()).length())),
                         2000);

    m_centralWidget->getDataView()->setData(coll->getData());

end:
    m_actions.startAction->setEnabled(true);
    m_actions.stopAction->setEnabled(false);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::stopAnalyze() throw ()
{
    m_analyzingActive = false;
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeForegroundColor()  throw ()
{
    QString currentColor = Settings::set().readEntry("UI/Foreground_Color_Line");
    QColor col = QColorDialog::getColor(currentColor, this);

    if (col.isValid()) {
        Settings::set().writeEntry("UI/Foreground_Color_Line", col.name());
        m_centralWidget->getDataView()->redrawData();
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeLeftColor()
    throw ()
{
    QString currentColor = Settings::set().readEntry("UI/Left_Marker_Color");
    QColor col = QColorDialog::getColor(currentColor, this);

    if (col.isValid()) {
        Settings::set().writeEntry("UI/Left_Marker_Color", col.name());
        m_centralWidget->getDataView()->redrawData();
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeRightColor()  throw ()
{
    QString currentColor = Settings::set().readEntry("UI/Right_Marker_Color");
    QColor col = QColorDialog::getColor(currentColor, this);

    if (col.isValid()) {
        Settings::set().writeEntry("UI/Right_Marker_Color", col.name());
        m_centralWidget->getDataView()->redrawData();
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeChannelAssignment()
    throw ()
{
    ProtocolSettingsDialog dialog(this);
    dialog.exec();
    updateProtocolEnabledStatus();
}

// -------------------------------------------------------------------------------------------------
void Tfla01::analyzeTriggered(QAction *action)
    throw ()
{
    QString protocol = action->data().toString();
    ProtocolAnalyzer *analyzer = AnalyzerManager::instance().getAnalyzerById(protocol);

    m_centralWidget->getDataView()->analyzeData(analyzer);
}

// -------------------------------------------------------------------------------------------------
void Tfla01::closeEvent(QCloseEvent* e)
{
    // write window layout
    Settings::set().writeEntry("Main Window/width", size().width());
    Settings::set().writeEntry("Main Window/height", size().height());
    Settings::set().writeEntry("Main Window/maximized", isMaximized());

    e->accept();
}


// -------------------------------------------------------------------------------------------------
void Tfla01::initMenubar() throw ()
{
    // ----- File ----------------------------------------------------------------------------------
    QMenu* fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(m_actions.openAction);
    fileMenu->addAction(m_actions.openRecentAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actions.saveAction);
    fileMenu->addAction(m_actions.exportAction);
    fileMenu->addAction(m_actions.saveViewAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actions.quitAction);

    // ----- Analyze -------------------------------------------------------------------------------
    QMenu* analyzeMenu = new QMenu(tr("&Analyze"), this);
    menuBar()->addMenu(analyzeMenu);
    analyzeMenu->addAction(m_actions.startAction);
    analyzeMenu->addAction(m_actions.stopAction);

    // ----- View ----------------------------------------------------------------------------------
    QMenu* viewMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(viewMenu);

    viewMenu->addAction(m_actions.zoomInAction);
    viewMenu->addAction(m_actions.zoomOutAction);
    viewMenu->addAction(m_actions.zoom1Action);
    viewMenu->addAction(m_actions.zoomFitAction);
    viewMenu->addAction(m_actions.zoomMarkersAction);

    // ----- Navigate ------------------------------------------------------------------------------
    QMenu* navigateMenu = new QMenu(tr("&Navigate"), this);
    menuBar()->addMenu(navigateMenu);

    navigateMenu->addAction(m_actions.navigatePos1Action);
    navigateMenu->addAction(m_actions.navigateEndAction);
    navigateMenu->addAction(m_actions.navigatePageLeftAction);
    navigateMenu->addAction(m_actions.navigatePageRightAction);
    navigateMenu->addAction(m_actions.navigateLeftAction);
    navigateMenu->addAction(m_actions.navigateRightAction);
    navigateMenu->addSeparator();
    navigateMenu->addAction(m_actions.jumpLeftAction);
    navigateMenu->addAction(m_actions.jumpRightAction);

    // ----- Settings---------------------------------------------------------------------------------
    QMenu* settingsMenu = new QMenu(tr("&Settings"), this);
    menuBar()->addMenu(settingsMenu);

    settingsMenu->addAction(m_actions.changeForegroundColorAction);
    settingsMenu->addAction(m_actions.changeLeftColorAction);
    settingsMenu->addAction(m_actions.changeRightColorAction);

#if HAVE_LIBIEEE1284
    m_portsMenu = new QMenu(tr("&Port"), this);
    settingsMenu->addMenu(m_portsMenu);
    for (int i = 0; i < m_actions.portActions.size(); ++i)
        m_portsMenu->addAction(m_actions.portActions[i]);
#endif

    // ----- Protocols -------------------------------------------------------------------------------
    QMenu *protocolsMenu = new QMenu(tr("&Protocols"), this);
    menuBar()->addMenu(protocolsMenu);

    protocolsMenu->addAction(m_actions.channelAssignmentAction);
    protocolsMenu->addSeparator();
    QList<QAction *> analyzeActions = m_actions.analyzeActions->actions();
    for (int i = 0; i < analyzeActions.size(); ++i)
        protocolsMenu->addAction(analyzeActions[i]);

    // right align on Motif
    menuBar()->addSeparator();

    // ----- Help ---------------------------------------------------------------------------------
    QMenu* helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(m_actions.helpAction);
    helpMenu->addSeparator();
    helpMenu->addAction(m_actions.aboutQtAction);
    helpMenu->addAction(m_actions.aboutAction);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::initActions()
    throw ()
{
    // ----- File ----------------------------------------------------------------------------------
    m_actions.openAction = new QAction(createIcon("stock_open", "document-open"),
                                       tr("&Open data..."), this);
    m_actions.openAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_O));

    m_actions.openRecentAction = new QAction(createIcon("stock_open_recent", "document-revert"),
                                             tr("Open &recent..."), this);
    m_actions.openRecentAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_R));

    m_actions.saveAction = new QAction(createIcon("stock_export", "document-save"),
                                       tr("&Save data..."), this);
    m_actions.saveAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_S));

    m_actions.exportAction = new QAction(createIcon("stock_export", "document-save-as"),
                                         tr("&Export data..."), this);
    m_actions.exportAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_E));

    m_actions.saveViewAction = new QAction(createIcon("stock_convert"),
                                           tr("&Save current plot..."), this);
    m_actions.exportAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_P));

    m_actions.quitAction = new QAction(createIcon("stock_exit", "application-exit"),
                                       tr("E&xit"), this);
    m_actions.quitAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Q));

    // ----- View ----------------------------------------------------------------------------------
    m_actions.zoomInAction = new QAction(createIcon("stock_zoom_in", "zoom-in"),
                                         tr("Zoom &In"), this);
	m_actions.zoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));

    m_actions.zoomOutAction = new QAction(createIcon("stock_zoom-out", "zoom-out"),
                                          tr("Zoom &Out"), this);
	m_actions.zoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));

    m_actions.zoomFitAction = new QAction(createIcon("stock_zoom-page-width", "zoom-fit-best"),
                                          tr("Zoom &Fit"), this);
    m_actions.zoomFitAction->setShortcut(QKeySequence(Qt::Key_F3));

	m_actions.zoom1Action = new QAction(createIcon("stock_zoom-1", "zoom-original"),
	                                    tr("Zoom &Default"), this);
	m_actions.zoom1Action->setShortcut(QKeySequence(Qt::Key_F2));

	QIcon zoomMarkersIcon(QPixmap(":/stock_zoom-optimal_16.png"));
	zoomMarkersIcon.addPixmap(QPixmap(":/stock_zoom-optimal_24.png"));
    m_actions.zoomMarkersAction = new QAction(zoomMarkersIcon, tr("Zoom to fit &markers"), this);
	m_actions.zoomMarkersAction->setShortcut(QKeySequence(Qt::Key_F4));

    // ----- Analyze -------------------------------------------------------------------------------
	m_actions.startAction = new QAction(createIcon("stock_redo", "edit-redo"),
	                                    tr("&Start"), this);
	m_actions.startAction->setShortcut(QKeySequence(Qt::Key_F5));

    m_actions.stopAction = new QAction(createIcon("stock_stop", "process-stop"),
                                       tr("&Stop"), this);
	m_actions.stopAction->setShortcut(QKeySequence(Qt::Key_F6));

    // ----- Navigate ------------------------------------------------------------------------------
    m_actions.navigatePos1Action = new QAction(createIcon("stock_first", "go-first"),
                                               tr("&Begin"), this);
	m_actions.navigatePos1Action->setShortcut(QKeySequence(Qt::Key_Home));

    m_actions.navigateEndAction = new QAction(createIcon("stock_last", "go-last"),
                                              tr("&End"), this);
	m_actions.navigateEndAction->setShortcut(QKeySequence(Qt::Key_End));

    m_actions.navigatePageLeftAction = new QAction(createIcon("stock_previous-page"),
                                                   tr("Page l&eft"), this);
	m_actions.navigatePageLeftAction->setShortcut(QKeySequence(Qt::Key_PageUp));

    m_actions.navigatePageRightAction = new QAction(createIcon("stock_next-page"),
                                                    tr("Page r&ight"), this);
	m_actions.navigatePageRightAction->setShortcut(QKeySequence(Qt::Key_PageDown));

    m_actions.navigateLeftAction = new QAction(createIcon("stock_left_arrow", "go-previous"),
                                               tr("&Left"), this);
	m_actions.navigateLeftAction->setShortcut(QKeySequence(Qt::Key_Left));

    m_actions.navigateRightAction = new QAction(createIcon("stock_right_arrow", "go-next"),
                                                tr("&Right"), this);
	m_actions.navigateRightAction->setShortcut(QKeySequence(Qt::Key_Right));

    m_actions.jumpLeftAction = new QAction(tr("&Jump to left marker"), this);
	m_actions.jumpLeftAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Left));

    m_actions.jumpRightAction = new QAction(tr("J&ump to right marker"), this);
	m_actions.jumpRightAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Right));

    m_actions.channelAssignmentAction = new QAction(tr("&Assign channels..."), this);

    // ----- Settings ------------------------------------------------------------------------------
    m_actions.changeForegroundColorAction = new QAction(createIcon("stock_3d-color-picker"),
                                                        tr("Change &foreground color..."), this);

    m_actions.changeLeftColorAction = new QAction(tr("Change color of &left marker..."), this);

    m_actions.changeRightColorAction = new QAction(tr("Change color of &right marker..."), this);

    // ----- Protocols -----------------------------------------------------------------------------
    QStringList protocolList = AnalyzerManager::instance().getAnalyzerNames();
    QStringList protocolIdList = AnalyzerManager::instance().getAnalyzerIds();
    QString baseString(tr("Analyze &%1..."));
    m_actions.analyzeActions = new QActionGroup(this);
    for (int i = 0; i < protocolList.size(); ++i) {
        QAction *newAction = m_actions.analyzeActions->addAction(baseString.arg(protocolList[i]));
        newAction->setData(protocolIdList[i]);
    }

    // ----- Help ----------------------------------------------------------------------------------
    m_actions.helpAction = new QAction(createIcon("stock_help", "help-browser"),
                                       tr("&Help"), this);
	m_actions.helpAction->setShortcut(QKeySequence(Qt::Key_F1));

	m_actions.aboutAction = new QAction(createIcon("stock_about", "help-about"), tr("&About..."), this);

    m_actions.aboutQtAction = new QAction(tr("About &Qt..."), this);

    //
    // create entries in the parallel port menu
    //
#if HAVE_LIBIEEE1284
    ParportList* list = ParportList::instance(0);
    for (int i = 0; i < list->getNumberOfPorts(); i++) {
        QAction *action = new QAction(list->getPort(i).getName(), this);
        action->setData(QVariant(i));
        action->setCheckable(true);
        action->setChecked(i == Settings::set().readNumEntry("Hardware/Parport"));
        m_actions.portActions.push_back(action);
    }
#endif
}


// -------------------------------------------------------------------------------------------------
void Tfla01::initToolbar()
    throw ()
{
    QToolBar* applicationToolbar = new QToolBar(tr("Application"), this);
    addToolBar(applicationToolbar);

    applicationToolbar->addAction(m_actions.quitAction);
    applicationToolbar->addSeparator();
    applicationToolbar->addAction(m_actions.startAction);
    applicationToolbar->addAction(m_actions.stopAction);
    applicationToolbar->addSeparator();
    applicationToolbar->addAction(m_actions.zoomInAction);
    applicationToolbar->addAction(m_actions.zoomOutAction);
    applicationToolbar->addAction(m_actions.zoom1Action);
    applicationToolbar->addAction(m_actions.zoomFitAction);
    applicationToolbar->addAction(m_actions.zoomMarkersAction);
    applicationToolbar->addSeparator();
    applicationToolbar->addAction(m_actions.navigatePos1Action);
    applicationToolbar->addAction(m_actions.navigateEndAction);
    applicationToolbar->addAction(m_actions.navigatePageLeftAction);
    applicationToolbar->addAction(m_actions.navigatePageRightAction);
    applicationToolbar->addAction(m_actions.navigateLeftAction);
    applicationToolbar->addAction(m_actions.navigateRightAction);
    applicationToolbar->addSeparator();
    applicationToolbar->addAction(m_actions.helpAction);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::connectSignalsAndSlots()
    throw ()
{

    connect(m_actions.openAction,                  SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(open()));
    connect(m_actions.openRecentAction,            SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(reopen()));
    connect(m_actions.saveAction,                  SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(save()));
    connect(m_actions.saveViewAction,              SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(saveScreenshot()));
    connect(m_actions.exportAction,                SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(exportToCSV()));
    connect(m_actions.quitAction,                  SIGNAL(triggered()),
            this,                                  SLOT(close()));
    connect(m_actions.helpAction,                  SIGNAL(triggered()),
            &m_help,                               SLOT(showHelp()));
    connect(m_actions.aboutAction,                 SIGNAL(triggered()) ,
            &m_help,                               SLOT(showAbout()));
    connect(m_actions.aboutQtAction,               SIGNAL(triggered()),
            qApp,                                  SLOT(aboutQt()));

    connect(m_actions.startAction,                 SIGNAL(triggered()),
            this,                                  SLOT(startAnalyze()));
    connect(m_actions.stopAction,                  SIGNAL(triggered()),
            this,                                  SLOT(stopAnalyze()));
    connect(m_actions.zoom1Action,                 SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(zoom1()) );
    connect(m_actions.zoomInAction,                SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(zoomIn()) );
    connect(m_actions.zoomOutAction,               SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(zoomOut()) );
    connect(m_actions.zoomFitAction,               SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(zoomFit()) );
    connect(m_actions.zoomMarkersAction,           SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(zoomMarkers()));
    connect(m_actions.changeForegroundColorAction, SIGNAL(triggered()),
            this,                                  SLOT(changeForegroundColor()));
    connect(m_actions.changeLeftColorAction,       SIGNAL(triggered()),
            this,                                  SLOT(changeLeftColor()));
    connect(m_actions.changeRightColorAction,      SIGNAL(triggered()),
            this,                                  SLOT(changeRightColor()));
    connect(m_actions.navigatePos1Action,          SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(pos1()));
    connect(m_actions.navigateEndAction,           SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(end()));
    connect(m_actions.navigateLeftAction,          SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(navigateLeft()));
    connect(m_actions.navigateRightAction,         SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(navigateRight()));
    connect(m_actions.navigatePageLeftAction,      SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(navigateLeftPage()));
    connect(m_actions.navigatePageRightAction,     SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(navigateRightPage()));
    connect(m_actions.jumpLeftAction,              SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(jumpToLeftMarker()));
    connect(m_actions.jumpRightAction,             SIGNAL(triggered()),
            m_centralWidget->getDataView(),        SLOT(jumpToRightMarker()));

    connect(m_actions.channelAssignmentAction,     SIGNAL(triggered()),
            this,                                  SLOT(changeChannelAssignment()));
    connect(m_actions.analyzeActions,              SIGNAL(triggered(QAction *)),
            this,                                  SLOT(analyzeTriggered(QAction *)));

    if (m_portsMenu)
        connect(m_portsMenu, SIGNAL(triggered(QAction *)), this, SLOT(portChanged(QAction *)));
}


// -------------------------------------------------------------------------------------------------
QIcon Tfla01::createIcon(const QString &tfla01Name, const QString &freedesktopName)
{
    QString smallIcon = tfla01Name + "_16.png";
    QString largeIcon = tfla01Name + "_24.png";

    QIcon fallbackIcon(QPixmap(":/" + smallIcon));
    fallbackIcon.addPixmap(QPixmap(":/" + largeIcon));

#if QT_VERSION >= 0x040600
    return QIcon::fromTheme(freedesktopName, fallbackIcon);
#else
    return fallbackIcon;
#endif
}


// vim: set sw=4 ts=4 tw=100:
