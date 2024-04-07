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
#include "analyzer.h"
#include "settings.h"

// -------------------------------------------------------------------------------------------------
QString ProtocolAnalyzer::getSettingsString(const QString &channelName) const throw ()
{
    return QString("%1/%2").arg( getId() ).arg( Settings::normalizePart(channelName) );
}

// -------------------------------------------------------------------------------------------------
QString ProtocolAnalyzer::getDisplayString(const QString &channelName) const throw ()
{
    return QString("%1 - %2").arg( getName() ).arg( channelName );
}

// -------------------------------------------------------------------------------------------------
bool ProtocolAnalyzer::readChannelSetting() throw ()
{
    Settings &settings = Settings::set();
    QStringList channels = getChannels();
    for (int i = 0; i < channels.size(); ++i) {
        QString channel = channels[i];
        QString settingsKey = getSettingsString(channel);
        int channelNumber = settings.readNumEntry(settingsKey, -1);
        if (channelNumber == -1)
            return false;

        m_channelMap[channel] = channelNumber;
    }

    // check for uniqueness
    QList<int> channelNumbers = m_channelMap.values();
    qSort(channelNumbers);
    for (int i = 0; i < channelNumbers.size()-1; ++i)
        if (channelNumbers[i] == channelNumbers[i+1])
            return false;

    return true;
}

// -------------------------------------------------------------------------------------------------
int ProtocolAnalyzer::getChannel(const QString &name) const
    throw ()
{
    return m_channelMap[name];
}

