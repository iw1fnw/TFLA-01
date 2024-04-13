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
#include <QSettings>
#include <QDir>
#include <QApplication>

#include "settings.h"
#include "global.h"

// -------------------------------------------------------------------------------------------------
Settings::Settings()
    noexcept
  : m_qSettings("bwalle.de", "tfla-01")
{
    m_qSettings.beginGroup("/tfla-01");

#define DEF_STRING(a, b) ( m_stringMap.insert( (a), (b) ) )
#define DEF_INTEGE(a, b) ( m_intMap.insert( (a), (b) ) )
#define DEF_DOUBLE(a, b) ( m_doubleMap.insert( (a), (b) ) )
#define DEF_BOOLEA(a, b) ( m_boolMap.insert( (a), (b) ) )

    DEF_BOOLEA("Main Window/maximized",          false);
    DEF_STRING("Main Window/SearchHistory",      "");
    DEF_STRING("Main Window/Layout",             "");
    DEF_INTEGE("Hardware/Parport",               0);
    DEF_STRING("Main Window/LastFile",           "");
    DEF_INTEGE("Measuring/Triggering/Minutes",   0);
    DEF_INTEGE("Measuring/Triggering/Seconds",   5);
    DEF_INTEGE("Measuring/Triggering/Value",     0);
    DEF_INTEGE("Measuring/Triggering/Mask",      0xff);
    DEF_INTEGE("Measuring/Number_Of_Skips",      0);
    DEF_STRING("UI/Foreground_Color_Line",       "yellow");
    DEF_STRING("UI/Background_Color",            "black");
    DEF_STRING("UI/Left_Marker_Color",           "red");
    DEF_STRING("UI/Right_Marker_Color",          "green");

#ifdef Q_WS_WIN
    DEF_STRING("General/Webbrowser",             "explorer");
#else
    DEF_STRING("General/Webbrowser",             "firefox");
#endif

#undef DEF_STRING
#undef DEF_INTEGE
#undef DEF_DOUBLE
#undef DEF_BOOLEA
}


// -------------------------------------------------------------------------------------------------
void Settings::writeEntry(const QString & key, const QString & value)
    noexcept
{
    m_qSettings.setValue(key, value);
}


// -------------------------------------------------------------------------------------------------
void Settings::writeEntry(const QString & key, double value)
    noexcept
{
    m_qSettings.setValue(key, value);
}


// -------------------------------------------------------------------------------------------------
void Settings::writeEntry(const QString & key, int value)
    noexcept
{
    m_qSettings.setValue(key, value);
}


// -------------------------------------------------------------------------------------------------
void Settings::writeEntry(const QString & key, bool value)
    noexcept
{
    m_qSettings.setValue(key, value);
}


// -------------------------------------------------------------------------------------------------
QString Settings::readEntry(const QString & key)
    noexcept
{
    QString string;
    if (m_qSettings.contains(key))
        string = m_qSettings.value(key).toString();
    else if (m_stringMap.contains(key))
        string = m_stringMap[key];
    else
        PRINT_DBG("Implicit default returned, key = %s", qPrintable(key));
    return string;
}


// -------------------------------------------------------------------------------------------------
int Settings::readNumEntry (const QString & key, int def)
    noexcept
{
    int ret = -1;
    if (m_qSettings.contains(key))
        ret = m_qSettings.value(key).toInt();
    else if (m_intMap.contains(key))
        ret = m_intMap[key];
    else if (def != 0)
        ret = def;
    else
        PRINT_TRACE("Implicit default returned, key = %s", qPrintable(key));
    return ret;
}


// -------------------------------------------------------------------------------------------------
double Settings::readDoubleEntry(const QString & key)
    noexcept
{
    double ret = 0.0;
    if (m_qSettings.contains(key))
        ret = m_qSettings.value(key).toDouble();
    else if (m_doubleMap.contains(key))
        ret = m_doubleMap[key];
    else
        PRINT_TRACE("Implicit default returned, key = %s", qPrintable(key));
    return ret;
}


// -------------------------------------------------------------------------------------------------
bool Settings::readBoolEntry(const QString & key)
    noexcept
{
    bool ret = false;
    if (m_qSettings.contains(key))
        ret = m_qSettings.value(key).toBool();
    else if (m_boolMap.contains(key))
        ret = m_boolMap[key];
    else
        PRINT_TRACE("Implicit default returned, key = %s", qPrintable(key));
    return ret;
}


// -------------------------------------------------------------------------------------------------
Settings& Settings::set()
    noexcept
{
    static Settings instanz;

    return instanz;
}

// -------------------------------------------------------------------------------------------------
QString Settings::normalizePart(const QString &string)
{
    QString copy(string);
    return copy.replace("/", "_");
}

// vim: set sw=4 ts=4 tw=100:
