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

#include <QLayout>
#include <QPixmap>
#include <QApplication>
#include <QScrollBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QCursor>
#include <QWheelEvent>
#include <QTextStream>

#include "settings.h"
#include "dataview.h"
#include "dataplot.h"
#include "tfla01.h"

#define SCROLL_SAMPLES_PER_LINE 3
#define LINESTEP_DIVISOR 5.0
#define MAX_SCROLL_VALUE 10000.0

// -------------------------------------------------------------------------------------------------
const quint32 DataView::DATFILE_MAGIC_NUMBER = 0x54464C41; // "TFLA" in ASCII
const quint32 DataView::DATFILE_CURRENT_VERSION = 0;

// -------------------------------------------------------------------------------------------------
DataView::DataView(QWidget* parent)
    throw ()
    : QWidget(parent)
    , m_scrollDivisor(1)
{
    QVBoxLayout *layout = new QVBoxLayout;
    m_dataPlot = new DataPlot(this, this);
    m_scrollBar = new WheelScrollBar(Qt::Horizontal, this);
    m_scrollBar->setTracking(false);

    layout->addWidget(m_dataPlot);
    layout->addWidget(m_scrollBar);
    setLayout(layout);

    connect(m_dataPlot,              SIGNAL(leftMarkerValueChanged(double)),
            this,                    SIGNAL(leftMarkerValueChanged(double)));
    connect(m_dataPlot,              SIGNAL(rightMarkerValueChanged(double)),
            this,                    SIGNAL(rightMarkerValueChanged(double)));
    connect(m_scrollBar,             SIGNAL(actionTriggered(int)),
            this,                    SLOT(navigate(int)));
    connect(m_scrollBar,             SIGNAL(valueChanged(int)),
            this,                    SLOT(scrollValueChanged(int)));
    connect(m_dataPlot,              SIGNAL(viewUpdated()),
            this,                    SLOT(updateScrollInfo()));
}


// -------------------------------------------------------------------------------------------------
void DataView::setData(const Data& data)
    throw ()
{
    // QScrollBar has a bug with very large values and large display width so that the
    // slider becomes too small (should be 90 %, is 5 %)
    // tested with values
    //   bar->setGeometry(0, 500, 500, 500);
    //   bar->setRange(0, 1727);
    //   bar->setPageStep(2962973);
    // to solve this, we connect to the pagestep / linestep signals directly and use a smaller
    // pagestep and range value here
    m_currentData = data;
    m_scrollDivisor =  m_currentData.bytes().size()  == 0
                       ? 1
                       : m_currentData.bytes().size() / 1000;

    m_dataPlot->setStartIndex(0);
    m_dataPlot->update();
    m_dataPlot->clearMarkers();
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomIn()
    throw ()
{
    if (m_dataPlot->getNumberOfDisplayedSamples() > 5)
        m_dataPlot->setZoomFactor(m_dataPlot->getZoomFactor() * 2.0);
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomOut()
    throw ()
{
    m_dataPlot->setZoomFactor(m_dataPlot->getZoomFactor() / 2.0);
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomFit()
    throw ()
{
    if (m_currentData.bytes().size() != 0) {
        m_dataPlot->setStartIndex(0);
        m_dataPlot->setZoomFactor( double(m_dataPlot->getCurrentWidthForPlot() - 1) /
                              m_dataPlot->getPointsPerSample() / m_currentData.bytes().size() );
    } else {
        Tfla01::instance()->statusBar()->showMessage(tr("Function only available if data is displayed."), 4000);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::zoom1() throw ()
{
    m_dataPlot->setZoomFactor(1.0);
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomMarkers() throw ()
{
    if (m_dataPlot->getLeftMarker() > 0 && m_dataPlot->getRightMarker() > 0) {
        double diff = DABS(m_dataPlot->getRightMarker() - m_dataPlot->getLeftMarker());
        m_dataPlot->setZoomFactor( double(m_dataPlot->getCurrentWidthForPlot() - 2) /
                              m_dataPlot->getPointsPerSample() / diff );
        m_dataPlot->setStartIndex(qMin(m_dataPlot->getLeftMarker(), m_dataPlot->getRightMarker()));
    } else {
        Tfla01::instance()->statusBar()->showMessage(tr("Function only available if both markers are set."), 4000);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::pos1() throw ()
{
    m_dataPlot->setStartIndex(0);
}


// -------------------------------------------------------------------------------------------------
void DataView::end() throw ()
{
    if (m_currentData.bytes().size() > 0)
        m_dataPlot->setStartIndex(m_currentData.bytes().size() -
                                  m_dataPlot->getNumberOfPossiblyDisplayedSamples() + 2);
}


// -------------------------------------------------------------------------------------------------
void DataView::wheelEvent(QWheelEvent* e)
{
    switch (e->modifiers()) {
        case Qt::ControlModifier:
            if (e->delta() > 0)
                zoomIn();
            else
                zoomOut();
            break;

        case Qt::ShiftModifier:
            if (e->delta() > 0)
                navigateLeftPage();
            else
                navigateRightPage();
            break;

        case Qt::NoModifier:
            if (e->delta() > 0)
                navigateLeft();
            else
                navigateRight();
            break;

        default:
            // do nothing
            break;
    }

    e->accept();
}


// -------------------------------------------------------------------------------------------------
void DataView::updateScrollInfo()
    throw ()
{
    int ps = m_dataPlot->getNumberOfDisplayedSamples();

    m_scrollBar->setRange(0, (m_currentData.bytes().size() - ps) / m_scrollDivisor);
    m_scrollBar->setValue(m_dataPlot->getStartIndex() / m_scrollDivisor);

    // set this to calculate the size
    if ((m_currentData.bytes().size() - ps) == 0)
        m_scrollBar->setPageStep(0);
    else
        m_scrollBar->setPageStep(ps / m_scrollDivisor);
    m_scrollBar->setSingleStep(qRound(ps / 10.0 / m_scrollDivisor));

#if 0
        PRINT_TRACE("div = %d", m_scrollDivisor);
        PRINT_TRACE("range = %d to %d", m_scrollBar->minValue(), m_scrollBar->maxValue());
        PRINT_TRACE("ps = %d", m_scrollBar->pageStep());
#endif
}


// -------------------------------------------------------------------------------------------------
void DataView::redrawData()
    throw ()
{
    m_dataPlot->update();
}


// -------------------------------------------------------------------------------------------------
void DataView::analyzeData(ProtocolAnalyzer *analyzer)
    throw ()
{
    if (m_currentData.bytes().size() == 0) {
        Tfla01::instance()->statusBar()->showMessage(tr("No data available. Aquire some data first or load a file."), 4000);
        return;
    }

    QString result;
    QTextStream resultStream(&result, QIODevice::ReadWrite);

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    try {
        analyzer->analyze(m_currentData, m_dataPlot->getLeftMarker(), m_dataPlot->getRightMarker(),resultStream);
    } catch (const TfError &err) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this,
            tr("TFLA-01"), tr("%1 analysis failed: %2").arg(analyzer->getName()).arg(err.what()),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QApplication::restoreOverrideCursor();
    AnalyzerResultDialog dialog(analyzer->getName(), this);
    dialog.setResult(result);
    dialog.exec();
}


// -------------------------------------------------------------------------------------------------
void DataView::scrollValueChanged(int value) throw ()
{
    if (value == m_scrollBar->maximum())
        end();
    else
        m_dataPlot->setStartIndex(value * m_scrollDivisor);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigate(int direction) throw ()
{
    int si;

    switch (direction) {
        case QAbstractSlider::SliderSingleStepAdd:
            m_dataPlot->setStartIndex( qMin(m_dataPlot->getStartIndex() +
                        qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples() / 10.0),
                        int(m_currentData.bytes().size())) );
            break;

        case QAbstractSlider::SliderSingleStepSub:
            m_dataPlot->setStartIndex( qMax(0,  m_dataPlot->getStartIndex() -
                        qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples() / 10.0)) );
            break;

        case QAbstractSlider::SliderPageStepAdd:
            si = qMin( m_dataPlot->getStartIndex() +
                    qRound((float)m_dataPlot->getNumberOfPossiblyDisplayedSamples()),
                    int(m_currentData.bytes().size()));
            m_dataPlot->setStartIndex(si);
            break;

        case QAbstractSlider::SliderPageStepSub:
            si = qMax(0,  m_dataPlot->getStartIndex() -
                    qRound((float)m_dataPlot->getNumberOfPossiblyDisplayedSamples()));
            m_dataPlot->setStartIndex(si);
            break;

        default:
            break;
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeft() throw ()
{
    navigate(QAbstractSlider::SliderSingleStepSub);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRight() throw ()
{
    navigate(QAbstractSlider::SliderSingleStepAdd);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeftPage() throw ()
{
    navigate(QAbstractSlider::SliderPageStepSub);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRightPage() throw ()
{
    navigate(QAbstractSlider::SliderPageStepAdd);
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToLeftMarker() throw ()
{
    if (m_dataPlot->getLeftMarker() >= 0)
        m_dataPlot->setStartIndex(qMax(0, m_dataPlot->getLeftMarker()));
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToRightMarker() throw ()
{
    if (m_dataPlot->getRightMarker() >= 0) {
        // 3 instead of 2 because in end() size() is lastElementIndex + 1
        m_dataPlot->setStartIndex(qMax(0, m_dataPlot->getRightMarker() -
                                       m_dataPlot->getNumberOfDisplayedSamples() + 3)  );
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::saveScreenshot() throw ()
{
    std::auto_ptr<QFileDialog> filedialog(new QFileDialog(this, tr("Choose file to save")));
    filedialog->setNameFilter(tr("PNG files (*.png)"));
    filedialog->setFileMode(QFileDialog::AnyFile);
    filedialog->setDirectory(QDir::currentPath());
    filedialog->setDefaultSuffix("png");
    filedialog->setAcceptMode(QFileDialog::AcceptSave);

    if (filedialog->exec() != QDialog::Accepted)
        return;

    QPixmap screenshot = m_dataPlot->getScreenshot();
    if (!screenshot.save(filedialog->selectedFiles().front(), "PNG"))
        QMessageBox::warning(this,
            tr("TFLA-01"), tr("Current view could not be saved. Maybe you "
                "you don't have permissions to that location."),
                QMessageBox::Ok, QMessageBox::NoButton);
}


// -------------------------------------------------------------------------------------------------
void DataView::exportToCSV()
    throw ()
{
    std::auto_ptr<QFileDialog> filedialog(new QFileDialog(this, tr("Choose file name for exported data")));
    filedialog->setNameFilter(tr("CSV files (*.csv)"));
    filedialog->setFileMode(QFileDialog::AnyFile);
    filedialog->setDirectory(QDir::currentPath());
    filedialog->setDefaultSuffix("csv");
    filedialog->setAcceptMode(QFileDialog::AcceptSave);

    if (filedialog->exec() != QDialog::Accepted)
        return;

    bool diffMode = QMessageBox::question(this, tr("TFLA-01"),
             tr("Should only the difference be saved? This created smaller files."),
             QMessageBox::Yes|QMessageBox::No,
             QMessageBox::No) == QMessageBox::Yes;
    QString fileName = filedialog->selectedFiles().front();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QTextStream stream(&file);
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    // write header
    stream << "number;1;2;3;4;5;6;7;8\n";

    unsigned int size = m_currentData.bytes().size();
    for (unsigned int i = 0; i < size; ++i) {
        unsigned char byte = m_currentData.bytes()[i];

        // keep the event loop running
        if (i % 100 == 0)
            qApp->processEvents();

        // check for a state change
        if (diffMode && i != 0 && i != (size - 1) &&
                byte == m_currentData.bytes()[i-1] &&
                byte == m_currentData.bytes()[i+1])
            continue;

        stream << i << ';';
        for (int j = 0; j < NUMBER_OF_BITS_PER_BYTE; ++j) {
            stream << (int)(bit_is_set(byte, j) ? 1 : 0);
            if (j != NUMBER_OF_BITS_PER_BYTE - 1)
                stream << ';';
        }

        stream << '\n';
    }

    QApplication::restoreOverrideCursor();

    file.close();
}


// -------------------------------------------------------------------------------------------------
void DataView::save()
    throw ()
{
    std::auto_ptr<QFileDialog> filedialog(new QFileDialog(this, tr("Choose file to save")));
    filedialog->setNameFilter(tr("DAT files (*.dat)"));
    filedialog->setFileMode(QFileDialog::AnyFile);
    filedialog->setDirectory(QDir::currentPath());
    filedialog->setDefaultSuffix("dat");
    filedialog->setAcceptMode(QFileDialog::AcceptSave);

    if (filedialog->exec() != QDialog::Accepted)
        return;

    Settings::set().writeEntry("Main Window/LastFile", filedialog->selectedFiles().front());

    QFile file(filedialog->selectedFiles().front());
    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_4_0);
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    unsigned int size = m_currentData.bytes().size();

    // write header
    stream << DATFILE_MAGIC_NUMBER;
    stream << DATFILE_CURRENT_VERSION;
    stream << (quint32)size;
    stream << (quint32)m_currentData.getMeasuringTime();


    for (unsigned int i = 0; i < size; ++i) {
        unsigned char byte = m_currentData.bytes()[i];
        stream << byte;
        // keep the event loop running
        if (i % 100 == 0)
            qApp->processEvents();
    }

    QApplication::restoreOverrideCursor();
    file.close();

    Tfla01::instance()->statusBar()->showMessage(
        tr("Successfully saved data to %1.").arg(filedialog->selectedFiles().front()), 4000);
}


// -------------------------------------------------------------------------------------------------
void DataView::loadfile(const QString &fileName)
    throw ()
{

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,
            tr("TFLA-01"), tr("Unable to open file '%1'.").arg(fileName),
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QDataStream stream(&file);

    quint32 size, time, version, magic_number;

    // read header
    stream >> magic_number;
    if (magic_number != DATFILE_MAGIC_NUMBER) {
        QMessageBox::warning(this,
            tr("TFLA-01"),
            tr("The specific file is not a TFLA-01 data file: Invalid magic number.").arg(fileName),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    stream >> version;
    if (version > DATFILE_CURRENT_VERSION) {
        QMessageBox::warning(this,
            tr("TFLA-01"),
            tr("The TFLA-01 data file is too new. Please update TFLA-01.").arg(fileName),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    stream >> size;
    stream >> time;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    m_currentData.setMeasuringTime(time);

    m_currentData.bytes().clear();

    unsigned char byte;
    for (unsigned int i = 0; i < size; ++i) {
        stream >> byte;

        m_currentData.bytes().push_back(byte);

        // keep the event loop running
        if (i % 100 == 0)
            qApp->processEvents();
    }

    QApplication::restoreOverrideCursor();
    file.close();
    setData(m_currentData);
}


// -------------------------------------------------------------------------------------------------
void DataView::open()
    throw ()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Choose file to open"), QDir::currentPath(),
                                            tr("DAT files (*.dat)"));
    if (fileName.isNull())
        return;
    Settings::set().writeEntry("Main Window/LastFile", fileName);
    loadfile(fileName);
}


// -------------------------------------------------------------------------------------------------
void DataView::reopen()
    throw ()
{
    QString fileName;
    fileName = Settings::set().readEntry("Main Window/LastFile");
    if (fileName.isEmpty())
        Tfla01::instance()->statusBar()->showMessage(tr("No recent file available to open."), 4000);
    else
        loadfile(fileName);
}


// vim: set sw=4 ts=4 tw=100 et:
