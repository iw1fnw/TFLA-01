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
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QFrame>

#include "centralwidget.h"
#include "dataview.h"

// -------------------------------------------------------------------------------------------------
CentralWidget::CentralWidget(QWidget* parent)
    noexcept
    : QFrame(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;
    m_dataView = new DataView(this);

    layout->setStretchFactor(m_dataView, 10);

    m_parametersBox = new ParameterBox(this);

    layout->addWidget(m_dataView);
    layout->addSpacing(20);
    layout->addWidget(m_parametersBox);
    layout->setStretchFactor(m_dataView, 1);
    layout->setStretchFactor(m_parametersBox, 0);

    connect(m_dataView,                SIGNAL(leftMarkerValueChanged(double)),
            m_parametersBox,           SLOT(setLeftMarker(double)));
    connect(m_dataView,                SIGNAL(rightMarkerValueChanged(double)),
            m_parametersBox,           SLOT(setRightMarker(double)));

    setLayout(layout);
    layout->setContentsMargins(15, 5, 15, 15);
}


// -------------------------------------------------------------------------------------------------
DataView* CentralWidget::getDataView() const
    noexcept
{
    return m_dataView;
}

// vim: set sw=4 ts=4 tw=100:
