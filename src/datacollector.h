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
#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include <QObject>
#include <QDateTime>
#include <QThread>

#include "hardware/parport.h"
#include "global.h"
#include "data.h"

/**
 * This class is to collect the data from the parallel port. The data is collected once, so
 * no update is possible. You set up the object, run the thread and once it finishes the
 * data are available. You may access the data in between, and you may stop collecting immediately.
 */
class DataCollector : public QThread
{
    public:
        static const int INITIAL_VECTOR_SPACE;

    public:
        DataCollector(unsigned int portNumber) noexcept;

        void setTriggering(bool enabled, unsigned char value = 0x00, unsigned char mask = 0x00) noexcept;
        bool isTriggeringEnabled() const noexcept;
        byte getTriggeringValue() const noexcept;
        byte getTriggeringMask() const noexcept;

        void setCollectingTime(int msecs) noexcept;
        int getCollectingTime() const noexcept;

        void setNumberOfSkips(int numberOfSkips) noexcept;
        int getNumberOfSkips() const noexcept;

        Data getData() const noexcept;
        QString getErrorString() const noexcept;

        void stop() noexcept;

    protected:
        void run();

    private:
        unsigned int  m_portNumber;
        bool          m_triggering;
        byte          m_triggeringMask;
        byte          m_triggeringValue;
        Data          m_data;
        int           m_collectingTime;
        QString       m_errorString;
        bool          m_stop;
        int           m_numberOfSkips;
};

#endif /* DATACOLLECTOR_H */

// vim: set sw=4 ts=4 tw=100:
