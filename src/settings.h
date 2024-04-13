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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
    public:
        Settings() noexcept;
        virtual ~Settings() { }

        void writeEntry(const QString & key, const QString & value) noexcept;
        void writeEntry(const QString & key, int value) noexcept;
        void writeEntry(const QString & key, bool value) noexcept;
        void writeEntry(const QString & key, double value) noexcept;

        QString readEntry(const QString & key) noexcept;
        int readNumEntry (const QString & key, int def=0) noexcept;
        double readDoubleEntry(const QString & key) noexcept;
        bool readBoolEntry(const QString & key) noexcept;

     public:
        static Settings& set() noexcept;

        static QString normalizePart(const QString &string);

    private:
        QSettings               m_qSettings;
        QMap<QString, QString>  m_stringMap;
        QMap<QString, int>      m_intMap;
        QMap<QString, bool>     m_boolMap;
        QMap<QString, double>   m_doubleMap;

    private:
        Settings(const Settings&);
        Settings& operator=(const Settings&);
};

#endif /* SETTINGS_H */

// vim: set sw=4 ts=4 tw=100:
