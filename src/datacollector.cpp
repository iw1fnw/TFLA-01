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
#include <sys/timeb.h>
#include <errno.h>

#include <vector>

#include <QObject>
#include <QDateTime>
#include <QApplication>
#include <QStatusBar>

#include "tfla01.h"
#include "datacollector.h"
#include "tfla01config.h"
#if HAVE_LIBIEEE1284
#  include "hardware/parportlist.h"
#endif

// -------------------------------------------------------------------------------------------------
const int DataCollector::INITIAL_VECTOR_SPACE   = 1000000;

// -------------------------------------------------------------------------------------------------
DataCollector::DataCollector(unsigned int port) throw ()
    : m_portNumber(port)
    , m_triggering(false)
    , m_triggeringMask(0)
    , m_triggeringValue(0)
    , m_collectingTime(0)
    , m_stop(false)
    , m_numberOfSkips(0)
{
    m_data.bytes().reserve(INITIAL_VECTOR_SPACE);
}

// -------------------------------------------------------------------------------------------------
void DataCollector::setTriggering(bool enabled, unsigned char value, unsigned char mask) throw ()
{
    m_triggering = enabled;
    m_triggeringValue = value;
    m_triggeringMask = mask;
}


// -------------------------------------------------------------------------------------------------
bool DataCollector::isTriggeringEnabled() const throw ()
{
    return m_triggering;
}


// -------------------------------------------------------------------------------------------------
unsigned char DataCollector::getTriggeringValue() const throw ()
{
    return m_triggeringValue;
}


// -------------------------------------------------------------------------------------------------
unsigned char DataCollector::getTriggeringMask() const throw ()
{
    return m_triggeringMask;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::setCollectingTime(int time) throw ()
{
    m_collectingTime = time;
}


// -------------------------------------------------------------------------------------------------
int DataCollector::getCollectingTime() const throw ()
{
    return m_collectingTime;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::setNumberOfSkips(int numberOfSkips) throw ()
{
    m_numberOfSkips = numberOfSkips;
}


// -------------------------------------------------------------------------------------------------
int DataCollector::getNumberOfSkips() const throw ()
{
    return m_numberOfSkips;
}


// -------------------------------------------------------------------------------------------------
Data DataCollector::getData() const throw ()
{
    return m_data;
}


// -------------------------------------------------------------------------------------------------
QString DataCollector::getErrorString() const throw ()
{
    return m_errorString;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::stop()  throw ()
{
    m_stop = true;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::run()
{
#if !HAVE_LIBIEEE1284
    m_errorString = QObject::tr("Compiled without parallel port support.");
#else

    try {
        // open the port and claim it
        ParportList* list = ParportList::instance();
        if (list->getNumberOfPorts() == 0) {
            m_errorString = QObject::tr("No ports available. Please check your installation.");
            return;
        } else if (int(m_portNumber) >= list->getNumberOfPorts() ) {
            m_errorString = QObject::tr("Port number %1 invalid. Please select a port in the "
                                        "\"Settings\" menu.").arg(m_portNumber);
            return;
        }
        Parport port = list->getPort(m_portNumber);

        // I tried F1284_EXCL here, but if I set this, claim failed with a system error
        port.open(0);
        port.claim();
        port.setDataDirection(true);

        //power up the buffer
        port.writeControl(Parport::CONTROL_NSTROBE);

        // wait for triggering
        if (m_triggering && m_triggeringMask != 0x00) {
            struct timeval timeout = { 1, 0 };

            while (!m_stop) {
                if (port.waitData(m_triggeringMask, m_triggeringValue, &timeout, true))
                    break;
            }
        }

        QTime start = QTime::currentTime();

        while ((start.msecsTo(QTime::currentTime()) <= m_collectingTime) && !m_stop) {
            for (int i = 0; i < 100 && !m_stop; i++) {
                for (int j = 0; j < m_numberOfSkips; j++)
                    port.readData();
                m_data.bytes().push_back(port.readData());
            }
        }

        m_data.setMeasuringTime(start.msecsTo(QTime::currentTime()));
        m_data.calculateLineStates();
    } catch (const ParportError& err) {
        PRINT_TRACE("Error occured while getting data: %s", qPrintable(err.what()));
        PRINT_DBG("OS error was %s", strerror(errno));
        m_errorString = err.what();
#ifdef Q_WS_WIN
        m_errorString += "<p>" +
            QObject::tr("Make sure that \"giveio.sys\" is installed correctly. Consult the documentation for more information.");
#endif
    } catch (const TfError& err) {
        PRINT_TRACE("Error occured while getting data: %s", qPrintable(err.what()));
        PRINT_DBG("OS error was %s", strerror(errno));
        m_errorString = err.what();
    }
#endif
}

// vim: set sw=4 ts=4 tw=100:
