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
#ifndef TFLA01_H
#define TFLA01_H

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QCloseEvent>

#include "centralwidget.h"
#include "help.h"
#include "analyzerresultdialog.h"
#include "updatechecker.h"

class Tfla01 : public QMainWindow
{
    Q_OBJECT

    public:
        static QIcon createIcon(const QString &tfla01Name, const QString &freedesktopName="");

    public:
        static Tfla01 *instance();

    protected:
        void initActions() throw ();
        void initMenubar() throw ();
        void initToolbar() throw ();
        void connectSignalsAndSlots() throw ();
        void closeEvent(QCloseEvent* evt);
        void portChange(int portNumber) throw ();
        void updateProtocolEnabledStatus() throw ();

    protected slots:
        void portChanged(QAction *action) throw ();
        void startAnalyze() throw ();
        void stopAnalyze() throw ();
        void changeForegroundColor() throw ();
        void changeLeftColor() throw ();
        void changeRightColor() throw ();
        void changeChannelAssignment() throw ();
        void analyzeTriggered(QAction *) throw ();

    private:
        static Tfla01 *m_instance;
        struct {
            QAction *saveViewAction;
            QAction *openAction;
            QAction *openRecentAction;
            QAction *saveAction;
            QAction *exportAction;
            QAction *quitAction;
            QAction *whatsThisAction;
            QAction *helpAction;
            QAction *aboutAction;
            QAction *aboutQtAction;
            QAction *zoomInAction;
            QAction *zoomOutAction;
            QAction *zoomFitAction;
            QAction *zoom1Action;
            QAction *zoomMarkersAction;
            QAction *startAction;
            QAction *stopAction;
            QAction *changeForegroundColorAction;
            QAction *changeLeftColorAction;
            QAction *changeRightColorAction;
            QAction *navigatePos1Action;
            QAction *navigateEndAction;
            QAction *navigateLeftAction;
            QAction *navigateRightAction;
            QAction *navigatePageLeftAction;
            QAction *navigatePageRightAction;
            QAction *jumpLeftAction;
            QAction *jumpRightAction;
            QAction *channelAssignmentAction;
            QList<QAction *> portActions;
            QActionGroup     *analyzeActions;
        }
        m_actions;
        CentralWidget  *m_centralWidget;
        QMenu          *m_portsMenu;
        Help           m_help;
        bool           m_analyzingActive;
        UpdateChecker  *m_updateChecker;

    private:
        Q_DISABLE_COPY(Tfla01);
        Tfla01() throw ();
        virtual ~Tfla01() {}
};

#endif /* TFLA01_H */

// vim: set sw=4 ts=4 tw=100:
