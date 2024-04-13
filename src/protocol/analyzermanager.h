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
#ifndef ANALYZERMANAGER_H
#define ANALYZERMANAGER_H

#include <QMap>
#include <QList>
#include "analyzer.h"

class AnalyzerManager {
    public:
        static AnalyzerManager &instance();

    protected:
        AnalyzerManager();
        virtual ~AnalyzerManager();

    public:
        void registerAnalyzer(ProtocolAnalyzer *analyzer) noexcept;
        QList<ProtocolAnalyzer *> getAnalyzers() const noexcept;
        QStringList getAnalyzerNames() const noexcept;
        QStringList getAnalyzerIds() const noexcept;
        ProtocolAnalyzer *getAnalyzerById(const QString &id) const noexcept;

        QStringList getSettingsStringList() const noexcept;
        QStringList getDisplayStringList() const noexcept;

    private:
        QMap<QString, ProtocolAnalyzer *> m_analyzers;
};

#endif /* ANALYZERMANAGER_H */
