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
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QMessageBox>

#include "protocol/analyzermanager.h"
#include "protocolsettingsdialog.h"
#include "settings.h"

// -------------------------------------------------------------------------------------------------
ProtocolSettingsDialog::ProtocolSettingsDialog(QWidget *parent)
    throw ()
    : QDialog(parent)
{
    setWindowTitle(tr("TFLA-01"));
    loadProtocolChannels();
    setupUI();
    loadSettings();
}

// -------------------------------------------------------------------------------------------------
void ProtocolSettingsDialog::setupUI()
    throw ()
{
    QHBoxLayout *verticalBox = new QHBoxLayout();

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Vertical);

    QGroupBox *groupBox = new QGroupBox(tr("Channel Assignments"));
    QGridLayout *groupGrid = new QGridLayout(this);

    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; ++i) {
        QString channelString(tr("Channel &%1:"));

        // create all the labels
        m_channelLabels[i] = new QLabel(channelString.arg(i), this);
        groupGrid->addWidget(m_channelLabels[i], i, 0);

        // create all the combo boxes
        m_protocolCombos[i] = new QComboBox(this);
        m_protocolCombos[i]->addItems(m_protocolChannels);
        groupGrid->addWidget(m_protocolCombos[i], i, 1);

        m_channelLabels[i]->setBuddy(m_protocolCombos[i]);
    }
    groupBox->setLayout(groupGrid);

    verticalBox->addWidget(groupBox);
    verticalBox->addSpacing(20);
    verticalBox->addWidget(m_buttons);
    setLayout(verticalBox);

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

// -------------------------------------------------------------------------------------------------
void ProtocolSettingsDialog::saveSettings()
    throw ()
{
    Settings &set = Settings::set();

    // check if a setting is twice
    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; ++i) {
        int value = m_protocolCombos[i]->currentIndex();
        if (value == 0)
            continue;

        for (int j = i+1; j < NUMBER_OF_BITS_PER_BYTE; ++j) {
            int otherValue = m_protocolCombos[j]->currentIndex();
            if (value == otherValue) {
                 QMessageBox::warning(this, tr("TFLA-01"),
                                      tr("It is not allowed to set the same protocol/channel combination "
                                         "for two input channels."));
                 return;
            }
        }
    }

    // zero all settings first
    QStringList settingsStringList = AnalyzerManager::instance().getSettingsStringList();
    for (int i = 0; i < settingsStringList.size(); ++i)
        set.writeEntry(settingsStringList[i], -1);

    // and then iterate over the channels
    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; ++i) {
        int value = m_protocolCombos[i]->currentIndex();
        if (value != 0)
            set.writeEntry(settingsStringList[value-1], i);
    }

    accept();
}

// -------------------------------------------------------------------------------------------------
void ProtocolSettingsDialog::loadProtocolChannels()
    throw ()
{
    m_protocolChannels << tr("(not assigned)");
    m_protocolChannels << AnalyzerManager::instance().getDisplayStringList();
}

// -------------------------------------------------------------------------------------------------
void ProtocolSettingsDialog::loadSettings()
    throw ()
{
    Settings &set = Settings::set();
    AnalyzerManager &analyzerManager = AnalyzerManager::instance();

    QStringList settingsStringList = analyzerManager.getSettingsStringList();
    for (int i = 0; i < settingsStringList.size(); ++i) {
        QString key = settingsStringList[i];
        int value = set.readNumEntry(key, -1);

        if (value != -1)
            m_protocolCombos[value]->setCurrentIndex(i+1);
    }
}

// -------------------------------------------------------------------------------------------------
QString ProtocolSettingsDialog::getProtocolChannelText(const QString &protocol, const QString &channel) const
    throw ()
{
    return QString("%1 - %2").arg(protocol).arg(channel);
}

// vim: set sw=4 ts=4 tw=100:
