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
#ifndef TRIGGERWIDGET_H
#define TRIGGERWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QDateTime>

#include "global.h"

class TriggerWidget : public QWidget
{
     Q_OBJECT

     public:
        TriggerWidget(QWidget* parent) noexcept;
        byte getMask() const noexcept;
        byte getValue() const noexcept;
        void setValue(byte mask, byte value) noexcept;

    protected slots:
        void valueChangedHandler() noexcept;

    signals:
        void valueChanged(byte mask, byte value);

    private:
        QCheckBox* m_checkboxes[NUMBER_OF_BITS_PER_BYTE];
};

#endif /* TRIGGERWIDGET_H */

// vim: set sw=4 ts=4 tw=100:
