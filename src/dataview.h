/*
 * Copyright (c) 2005-2010, Bernhard Walle
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
#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QWheelEvent>

#include "dataplot.h"
#include "global.h"
#include "data.h"
#include "wheelscrollbar.h"
#include "protocol/analyzer.h"

class DataView : public QWidget
{
    Q_OBJECT

    friend class DataPlot;

    public:
        static const quint32 DATFILE_MAGIC_NUMBER;
        static const quint32 DATFILE_CURRENT_VERSION;

    public:
        DataView(QWidget* parent) throw ();
        virtual ~DataView() {}

        void setData(const Data& data) throw ();
        void redrawData() throw ();
        void analyzeData(ProtocolAnalyzer *analyzer) throw ();

    public slots:
        void zoomIn() throw ();
        void zoomOut() throw ();
        void zoomFit() throw ();
        void zoom1() throw ();
        void zoomMarkers() throw ();

        void pos1() throw ();
        void end() throw ();
        void navigate(int direction) throw ();
        void navigateLeft() throw ();
        void navigateRight() throw ();
        void navigateLeftPage() throw ();
        void navigateRightPage() throw ();
        void jumpToLeftMarker() throw ();
        void jumpToRightMarker() throw ();
        void saveScreenshot() throw ();
        void exportToCSV() throw ();
        void save() throw ();
        void loadfile(const QString &fileName) throw ();
        void open() throw ();
        void reopen() throw ();

    signals:
        void leftMarkerValueChanged(double value);
        void rightMarkerValueChanged(double value);
        void diffValueChanged(double value);

    protected:
        void wheelEvent(QWheelEvent* e);

    protected slots:
        void updateScrollInfo() throw ();
        void scrollValueChanged(int value) throw ();

    private:
        DataPlot*       m_dataPlot;
        WheelScrollBar* m_scrollBar;
        Data            m_currentData;
        int             m_scrollDivisor;
};

#endif /* DATAVIEW_H */

// vim: set sw=4 ts=4 tw=100:
