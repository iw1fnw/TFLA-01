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
#ifndef ANALYZER_H
#define ANALYZER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QTextStream>

#include "tferror.h"

class Data;

class ProtocolAnalyzer
{
    public:
        virtual ~ProtocolAnalyzer() {}

    public:
        virtual QString getId() const noexcept = 0;
        virtual QString getName() const noexcept = 0;
        virtual QStringList getChannels() const noexcept = 0;

        QString getSettingsString(const QString &channelName) const noexcept;
        QString getDisplayString(const QString &channelName) const noexcept;

        bool readChannelSetting() noexcept;
        int getChannel(const QString &name) const noexcept;

        virtual void analyze(const Data &data, int start, int end, QTextStream &output) = 0;

    private:
        QMap<QString, int> m_channelMap;
};

#endif /* ANALYZER_H */
