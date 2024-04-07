/*
 * Copyright (c) 2010, Bernhard Walle
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
#ifndef PROTOCOLSETTINGSDIALOG_H
#define PROTOCOLSETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QStringList>

#include "global.h"

class ProtocolSettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        ProtocolSettingsDialog(QWidget *parent) throw ();

    protected:
        void setupUI() throw ();
        void loadProtocolChannels() throw ();
        void loadSettings() throw ();
        QString getProtocolChannelText(const QString &protocol, const QString &channel) const throw ();

    protected slots:
        void saveSettings() throw ();

    private:
        QLabel              *m_channelLabels[NUMBER_OF_BITS_PER_BYTE];
        QComboBox           *m_protocolCombos[NUMBER_OF_BITS_PER_BYTE];
        QDialogButtonBox    *m_buttons;
        QStringList         m_protocolChannels;
};

#endif /* PROTOCOLSETTINGSDIALOG_H */

// vim: set sw=4 ts=4 tw=100:
