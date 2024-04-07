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
#include <QWheelEvent>

#include "wheelscrollbar.h"

// -------------------------------------------------------------------------------------------------
WheelScrollBar::WheelScrollBar(QWidget * parent)
    : QScrollBar(parent)
{}


// -------------------------------------------------------------------------------------------------
WheelScrollBar::WheelScrollBar(Qt::Orientation orientation, QWidget* parent)
    : QScrollBar(orientation, parent)
{}


// -------------------------------------------------------------------------------------------------
void WheelScrollBar::wheelEvent(QWheelEvent* e)
{
    switch (e->modifiers()) {
        case Qt::ShiftModifier:
            if (e->delta() > 0)
                emit actionTriggered(QAbstractSlider::SliderPageStepSub);
            else
                emit actionTriggered(QAbstractSlider::SliderPageStepAdd);
            break;

        case Qt::NoModifier:
            if (e->delta() > 0)
                emit actionTriggered(QAbstractSlider::SliderSingleStepSub);
            else
                emit actionTriggered(QAbstractSlider::SliderSingleStepAdd);
            break;

        default:
            // do nothing
            break;
    }

    e->accept();
}


// vim: set sw=4 ts=4 tw=100:
