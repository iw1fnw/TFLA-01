/*
 * Copyright (c) 2010, Bernhard Walle (converted to the new interface)
 * Copyright (c) 2010, kai.dorau@gmx.net (initial implementation)
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
#include <iomanip>
#include <sstream>

#include <QApplication>
#include <QDebug>

#include "spi.h"
#include "data.h"

/*
 * SPI interface
 *
 * SDO - Serial Data Out
 * SDI - Serial Data In
 * SCK - Serial Clock
 *
 * Mode  CPOL  CPHA
 * 0     0     0          <--- USED
 * 1     0     1
 * 2     1     0
 * 3     1     1
 *
 *
 * USED
 *
 *          +----+    +----+
 * CLK      |    |    |    |
 *       ---+    +----+    +---
 *          |         |
 *        +----+    +----+
 * SDOI   | |  |    | |  |
 *       -+ |  +----+ |  +---
 *          |         |
 */

// -------------------------------------------------------------------------------------------------
QString SpiAnalyzer::getName() const
    throw ()
{
    return "SPI";
}

// -------------------------------------------------------------------------------------------------
QString SpiAnalyzer::getId() const
    throw ()
{
    return "SPI";
}

// -------------------------------------------------------------------------------------------------
QStringList SpiAnalyzer::getChannels() const
    throw ()
{
    return QStringList() << "SDI/SDO" << "SCK";
}

// -------------------------------------------------------------------------------------------------
void SpiAnalyzer::analyze(const Data &data, int start, int end, QTextStream &output)
    throw (TfError)
{
    // get current data pool
    m_currentData = data;

    // set mask ans size of signales CLK and SDA
    setSPIMasks();
    setSPISize(end);

    if (start == -1)
        m_currentPosition = 0;
    else
        m_currentPosition = start;
    int bitCnt = 7;
    int byte = 0;

    int pulse, sda;

    // start condition
    if (getSPIStart() == 0) {
        // exception! the measurment started with
        // high CLK!!! In this case we guess, that the
        // rising edge of CLK has been occurred!

        sda = getSDABit();          // get SDA bit
        byte |= (sda << bitCnt);    // build the byte
        bitCnt--;                   // dec bit count
    }

    output << "Start SPI Stream (For defining the starting point, set left cursor)\n";

    //  for(val=m_currentPosition;val<500;val++)
    //  std::cout << "DEB -> Data: " << int(m_currentData.bytes()[val]) << "; " << int(val) << "\n";

    // oki, let's dance..
    while (m_currentPosition < m_size) {

        pulse = clkPulse();                         // search for the rising edge of CLK
        sda = getSDABit();                          // if found, get SDA bit
        if (sda == -1)
            break;
        if (pulse) {                                // if pulse found
            byte |= (sda << bitCnt);                // build the byte
            bitCnt--;                               // dec bit count

            if (bitCnt < 0) {                       // if 8 bits read...
                // print out SPI values
                output << binRepByte((unsigned char)byte) << "\n";
                bitCnt = 7;                         // ...next 8 bits
                byte = 0;
            }
        }

        // keep the event loop running
        if (m_currentPosition % 100 == 0)
            qApp->processEvents();
    }

    output << "End SPI Stream\n";
}

// -------------------------------------------------------------------------------------------------
void SpiAnalyzer::setSPIMasks() throw ()
{
    m_SCK_Mask = 1 << getChannel("SCK");
    qDebug() << "MASK -> sck_mask: " << int(m_SCK_Mask);
    m_SDI_SDO_Mask = 1 << getChannel("SDI/SDO");
    qDebug() << "MASK -> sdi_sdo_mask: " << int(m_SDI_SDO_Mask);
    m_SPI_Mask = m_SCK_Mask|m_SDI_SDO_Mask;
    qDebug() << "MASK -> spi_mask: " << int(m_SPI_Mask);
}

// -------------------------------------------------------------------------------------------------
void SpiAnalyzer::setSPISize(int end) throw ()
{
    if (end == -1)
        m_size = m_currentData.bytes().size();
    else
        m_size = end;
    qDebug() << "SIZE -> m_size: " << m_size;
}

// -------------------------------------------------------------------------------------------------
int SpiAnalyzer::getSPIStart() throw ()
{
    qDebug() << "START -> Start";

    if (m_currentData.bytes()[m_currentPosition] & m_SCK_Mask) {
        return 0;
    }
    return 1;
}


// -------------------------------------------------------------------------------------------------
int SpiAnalyzer::clkPulse() throw ()
{
    qDebug() << "PULSE -> Pulse";

    // search SCL Down
    while ((m_currentPosition < m_size) &&
           ((m_currentData.bytes()[m_currentPosition] & m_SCK_Mask) != 0))
        m_currentPosition++;

    // search SCL Up (Mode 0)
    while ((m_currentPosition < m_size) &&
           ((m_currentData.bytes()[m_currentPosition] & m_SCK_Mask) != m_SCK_Mask))
        m_currentPosition++;

    qDebug() << "PULSE -> CLK: " << int(m_currentPosition);

    if (m_currentPosition >= m_size)
        return 0;

    return 1;
}


// -------------------------------------------------------------------------------------------------
int SpiAnalyzer::getSDABit() throw()
{
    if (m_currentPosition >= m_size)
        return -1;

    // get SDA bit regarding rising edge of CLK
    if (m_currentData.bytes()[m_currentPosition] & m_SDI_SDO_Mask)
        return 1;
    return 0;
}


// -------------------------------------------------------------------------------------------------
QString SpiAnalyzer::binRepByte(unsigned char val) throw()
{
    std::stringstream ss;

    // output
    ss << std::setw(10) << m_currentPosition << " -> (" << std::setw(2) << std::hex << int(val) << "): ";
    for (unsigned char bit = 1<<(CHAR_BIT-1); bit; bit >>= 1) {
        if (val & bit)
            ss << 1;
        else
            ss << 0;
    }
    return QString::fromStdString(ss.str());
}
