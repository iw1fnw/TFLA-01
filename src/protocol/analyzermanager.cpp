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
#include "analyzermanager.h"
#include "spi.h"
#include "i2c.h"

// -------------------------------------------------------------------------------------------------
AnalyzerManager &AnalyzerManager::instance()
{
    static AnalyzerManager instance;
    return instance;
}

// -------------------------------------------------------------------------------------------------
AnalyzerManager::AnalyzerManager()
{
    registerAnalyzer(new I2cAnalyzer());
    registerAnalyzer(new SpiAnalyzer());
}

// -------------------------------------------------------------------------------------------------
AnalyzerManager::~AnalyzerManager()
{
    for (QMap<QString, ProtocolAnalyzer *>::iterator it = m_analyzers.begin(); it != m_analyzers.end(); ++it)
        delete it.value();
    m_analyzers.clear();
}

// -------------------------------------------------------------------------------------------------
void AnalyzerManager::registerAnalyzer(ProtocolAnalyzer *analyzer) noexcept
{
    m_analyzers[analyzer->getId()] = analyzer;
}

// -------------------------------------------------------------------------------------------------
QList<ProtocolAnalyzer *> AnalyzerManager::getAnalyzers() const noexcept
{
    return m_analyzers.values();
}

// -------------------------------------------------------------------------------------------------
QStringList AnalyzerManager::getAnalyzerNames() const noexcept
{
    QStringList names;
    for (QMap<QString, ProtocolAnalyzer *>::const_iterator it = m_analyzers.begin(); it != m_analyzers.end(); ++it)
        names.push_back(it.value()->getName());

    return names;
}

// -------------------------------------------------------------------------------------------------
QStringList AnalyzerManager::getAnalyzerIds() const noexcept
{
    return m_analyzers.keys();
}

// -------------------------------------------------------------------------------------------------
ProtocolAnalyzer *AnalyzerManager::getAnalyzerById(const QString &id) const noexcept
{
    return m_analyzers.value(id, NULL);
}

// -------------------------------------------------------------------------------------------------
QStringList AnalyzerManager::getSettingsStringList() const noexcept
{
    QStringList ret;
    for (QMap<QString, ProtocolAnalyzer *>::const_iterator it = m_analyzers.begin(); it != m_analyzers.end(); ++it) {
        ProtocolAnalyzer *analyzer = it.value();

        QStringList channels = analyzer->getChannels();
        for (QStringList::const_iterator channel_it = channels.begin(); channel_it != channels.end(); ++channel_it) {
            QString channelName = *channel_it;
            ret.push_back(analyzer->getSettingsString(channelName));
        }
    }

    return ret;
}

// -------------------------------------------------------------------------------------------------
QStringList AnalyzerManager::getDisplayStringList() const noexcept
{
    QStringList ret;
    for (QMap<QString, ProtocolAnalyzer *>::const_iterator it = m_analyzers.begin(); it != m_analyzers.end(); ++it) {
        ProtocolAnalyzer *analyzer = it.value();

        QStringList channels = analyzer->getChannels();
        for (QStringList::const_iterator channel_it = channels.begin(); channel_it != channels.end(); ++channel_it) {
            QString channelName = *channel_it;
            ret.push_back(analyzer->getDisplayString(channelName));
        }
    }

    return ret;
}
