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
#include <QLayout>
#include <QBoxLayout>
#include <QLabel>

#include "triggerwidget.h"

// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
TriggerWidget::TriggerWidget(QWidget* parent)
    noexcept
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;

    // create the checkboxes with their labels
    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++) {
        QWidget *box = new QWidget;
        QVBoxLayout *boxLayout = new QVBoxLayout;

        m_checkboxes[i] = new QCheckBox(QString::number(i+1));
        m_checkboxes[i]->setTristate(true);
        m_checkboxes[i]->setCheckState(Qt::PartiallyChecked);

        boxLayout->addWidget(m_checkboxes[i]);
        boxLayout->setContentsMargins(0, 0, 0, 0);
        box->setLayout(boxLayout);

        connect(m_checkboxes[i], SIGNAL(stateChanged(int)), SLOT(valueChangedHandler()));
        layout->addWidget(box);
        layout->setStretchFactor(box, 0);
    }

    layout->addStretch(5);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}


// -------------------------------------------------------------------------------------------------
byte TriggerWidget::getMask() const
    noexcept
{
    byte ret = 0;

    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++)
        ret |= (m_checkboxes[i]->checkState() != Qt::PartiallyChecked) << i;

    PRINT_TRACE("getMask, return %x", ret);
    return ret;
}


// -------------------------------------------------------------------------------------------------
byte TriggerWidget::getValue() const
    noexcept
{
    byte ret = 0;

    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++)
        ret |= (m_checkboxes[i]->checkState() == Qt::Checked) << i;

    PRINT_TRACE("getValue, return %x", ret);
    return ret;
}


// -------------------------------------------------------------------------------------------------
void TriggerWidget::valueChangedHandler()
    noexcept
{
    emit valueChanged(getMask(), getValue());
}


// -------------------------------------------------------------------------------------------------
void TriggerWidget::setValue(byte value, byte mask)
    noexcept
{
    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++) {
        if (!(mask & (1 << i)))
            m_checkboxes[i]->setCheckState(Qt::PartiallyChecked);
        else
            m_checkboxes[i]->setChecked(value & (1<< i));
    }
}

// vim: set sw=4 ts=4 tw=100:
