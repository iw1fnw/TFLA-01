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

#include "i2c.h"
#include "data.h"

// -------------------------------------------------------------------------------------------------
#define I2C_MAX_BYTE_VALUE 0xFF

#define I2C_STOP  0xF100
#define I2C_START 0xF200
#define I2C_ACK   0xF300
#define I2C_NACK  0xF400

#define I2C_ERROR 0xF500
#define I2C_ERROR_BYTE_SIZE 0xF501

// Some I2C circuits
#define PCF8574  0x40
#define PCF8574A 0x70
#define DS1307   0xD0

#define DS_CTRL_REG_NOT_SET 0xFF
#define DS_CTRL_REG_READY   0xFE


// -------------------------------------------------------------------------------------------------
QString I2cAnalyzer::getName() const noexcept
{
    return QString::fromUtf8("I²C");
}

// -------------------------------------------------------------------------------------------------
QString I2cAnalyzer::getId() const noexcept
{
    return "I2C";
}

// -------------------------------------------------------------------------------------------------
QStringList I2cAnalyzer::getChannels() const noexcept
{
    return QStringList() << "SDA" << "SCL";
}

// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::analyze(const Data &data, int start, int end, QTextStream &output)
{
    int val;
    ds_cont_reg = DS_CTRL_REG_NOT_SET;

    m_currentData = data;

    setI2C_Masks();
    setSize(end);

    if (start == -1)
        m_currentPosition = 0;
    else
        m_currentPosition = start;

    // search for initial Start
    if (getStart())
        writeCmd(output," - Start\n");
    while (m_currentPosition < m_size) {
        // Get byte
        val = getByte();
        switch (val & 0xFFF0) {
            case PCF8574:
            case PCF8574A:
                processPCF8574(val, output);
                break;

            case DS1307:
                processRTC(val, output);
                break;

            case I2C_START:
                writeCmd(output, " - Start\n");
                break;

            case I2C_STOP:
                writeCmd(output, " - Stop\n\n");
                break;

            default:
                processUnknowIC(val, output);
                break;
        }

        // keep the event loop running
        if (m_currentPosition % 100 == 0)
            qApp->processEvents();
    }

    QApplication::restoreOverrideCursor();
}

// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::setI2C_Masks() noexcept
{
    m_SCL_Mask = 1 << getChannel("SCL");
    m_SDA_Mask = 1 << getChannel("SDA");
    m_I2C_Mask = m_SCL_Mask | m_SDA_Mask;
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::setSize(int end) noexcept
{
    if (end == -1)
        m_size = m_currentData.bytes().size();
    else
        m_size = end;
}


// -------------------------------------------------------------------------------------------------
int I2cAnalyzer::getStart() noexcept
{
    while (m_currentPosition + 1 < m_size) {
        if (((m_currentData.bytes()[m_currentPosition] & m_I2C_Mask) == m_I2C_Mask) &&
            ((m_currentData.bytes()[m_currentPosition + 1] & m_I2C_Mask) == m_SCL_Mask)){
            m_currentPosition++;
            return true;
        }
        m_currentPosition++;
    }
    return false;
}


// -------------------------------------------------------------------------------------------------
int I2cAnalyzer::Clock_Pulse() noexcept
{
    unsigned char SDA_i;

    //search SCL Down
    while ((m_currentPosition < m_size) &&
           ((m_currentData.bytes()[m_currentPosition] & m_SCL_Mask) != 0))
        m_currentPosition++;

    //search SCL Up
    while ((m_currentPosition < m_size) &&
           ((m_currentData.bytes()[m_currentPosition] & m_SCL_Mask) != m_SCL_Mask))
        m_currentPosition++;

    // save initial SDA state
    if (m_currentPosition < m_size)
        SDA_i = m_currentData.bytes()[m_currentPosition] & m_SDA_Mask;

    // search SCL or SDA change
    while ((m_currentPosition < m_size) &&
           ((m_currentData.bytes()[m_currentPosition] & m_I2C_Mask) == (SDA_i | m_SCL_Mask)))
        m_currentPosition++;

    if (m_currentPosition < m_size) {
        // clock is high and data change
        if (((m_currentData.bytes()[m_currentPosition] & m_SCL_Mask) != 0) &&
            ((m_currentData.bytes()[m_currentPosition] & m_SDA_Mask) != SDA_i)){
            //data change
            if ((m_currentData.bytes()[m_currentPosition] & m_SDA_Mask) == m_SDA_Mask)
                //stop SDA Up
                return I2C_STOP;
            else
                //start SDA Down
                return I2C_START;
        } else {
            if (SDA_i == m_SDA_Mask)
                // bit up
                return 1;
            else
                // bit down
                return 0;
        }
    }

    return I2C_ERROR;
}


// -------------------------------------------------------------------------------------------------
int I2cAnalyzer::getByte() noexcept
{
    int bit_value = 0;
    int ret = 0;
    int n_bit = 7;

    while ((n_bit >= 0) && (bit_value <= I2C_MAX_BYTE_VALUE)){
        bit_value = Clock_Pulse();
        if (bit_value == 1)
            ret |= (1 << n_bit);
        n_bit--;
    }

    if (n_bit != -1) {
        // unexpected byte size
        return I2C_ERROR_BYTE_SIZE;
    }

    return ret;
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::writeCmd(QTextStream &output, QString text) noexcept
{
     output << QString::number(m_currentData.getMsecsForSample(m_currentPosition)) << text;
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::writeValue(QTextStream &output, int val) noexcept
{
    // Time - Bin - Hex - Dec
    output << QString::number( m_currentData.getMsecsForSample(m_currentPosition));
    output << " - b" << QString("%1").arg(val, 8, 2, QLatin1Char('0'));
    output << " - 0x" << QString("%1").arg(val, 2, 16, QLatin1Char('0'));
    output << " - " << QString::number(val) << "\n";
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::Decode_RTC(QTextStream &output,int val) noexcept
{
    output << QString::number( m_currentData.getMsecsForSample(m_currentPosition));

    switch (ds_cont_reg){
        case 0:
            val = (val & 0xF) + (((val >> 4) & 7) * 10);
            output << "- Seconds - " << QString::number(val) << "\n";
            break;
        case 1:
            val = (val & 0xF) + (((val >> 4) & 7) * 10);
            output << "- Minuts - " << QString::number(val) << "\n";
            break;
        case 2:
            val = (val & 0xF) + (((val >> 4) & 3) * 10);
            output << "- Hours - " << QString::number(val) << "\n";
            break;
        case 3:
            val = (val & 3);
            output << "- WDay - " << QString::number(val) << "\n";
            break;
        case 4:
            val = (val & 0xF) + (((val >> 4) & 3) * 10);
            output << "- Day - " << QString::number(val) << "\n";
            break;
        case 5:
            val = (val & 0xF) + (((val >> 4) & 1) * 10);
            output << "- Month - " << QString::number(val) << "\n";
            break;
        case 6:
            val = (val & 0xF) + (((val >> 4) & 4) * 10);
            output << "- Year - " << QString::number(val) << "\n";
            break;
        default :
            output << " - b" << QString("%1").arg(val,8,2,QLatin1Char('0'));
            output << " - 0x" << QString("%1").arg(val,2,16,QLatin1Char('0'));
            output << " - " << QString::number(val) << "\n";
            break;
    }
    ds_cont_reg++;
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::writeError(int val, QTextStream &output) noexcept
{
    switch (val) {
        case I2C_START:
            writeCmd(output, " - Start\n");
            break;
        case I2C_STOP:
            writeCmd(output, " - Stop\n\n");
            break;

        case I2C_ERROR:
            writeCmd(output, " - Error\n\n");
            if (getStart())
                writeCmd(output, " - Start\n");
            break;

        case I2C_ERROR_BYTE_SIZE:
            writeCmd(output, " - Error byte lenght\n\n");
            if (getStart())
                writeCmd(output, " - Start\n");
            break;
    }
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::writeACK(int val, QTextStream &output) noexcept
{
    switch (val) {
        case 0:
            writeCmd(output, " - ACK\n");
            break;

        case 1:
            writeCmd(output, " - NACK\n");
            break;

        default:
            writeCmd(output, " - Error\n\n");
            if (getStart())
                writeCmd(output, " - Start\n");
            break;
    }
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::processUnknowIC(int val, QTextStream &output) noexcept
{
    do {
        if (val > I2C_MAX_BYTE_VALUE) {
            writeError(val, output);
        } else {
            writeValue(output, val);

            // get ACK
            val = Clock_Pulse();
            if (val < 2){ // 0 or 1 -> ACK or NACK
                writeACK(val, output);
                val = getByte();
            } else{
                writeError(val, output);
            }
        }
    } while (val <= I2C_MAX_BYTE_VALUE);
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::processPCF8574(int val, QTextStream &output) noexcept
{
    //writeAddrs
    if ((val & 0x01) == 0x01) {
        writeCmd(output, " - Read from PCF8574 at address "+QString::number((val >> 1)&0x3)+"\n");
    } else {
        writeCmd(output, " - Write to PCF8574 at address "+QString::number((val >> 1)&0x3)+"\n");
    }
    writeValue(output, val);

    // get ACK
    val = Clock_Pulse();
    writeACK(val, output);
    if (val < 2){
        do {
            val = getByte();
            if (val > 0xFF) {
                writeError(val, output);
            } else {
                writeValue(output, val);
                val = Clock_Pulse();
                writeACK(val, output);
            }
        } while (val <= I2C_MAX_BYTE_VALUE);
    }
}


// -------------------------------------------------------------------------------------------------
void I2cAnalyzer::processRTC(int val, QTextStream &output) noexcept
{
    if ((val & 0x01) == 0x01) {
        writeCmd(output, " - Read from RTC at address "+QString::number((val >> 1)&0x3)+"\n");
    } else {
        writeCmd(output, " - Write to RTC at address "+QString::number((val >> 1)&0x3)+"\n");
        // next byte must be control register address
        ds_cont_reg = DS_CTRL_REG_READY;
    }
    writeValue(output, val);

    val = Clock_Pulse();
    writeACK(val, output);
    if (val < 2) {
        do {
            val = getByte();
            if (val > I2C_MAX_BYTE_VALUE) {
                writeError(val, output);
            } else {
                if (ds_cont_reg == DS_CTRL_REG_READY){
                    //control register address
                    ds_cont_reg = val;
                    writeValue(output, val);
                } else {
                    // RTC values
                    Decode_RTC(output, val);
                }
                if (ds_cont_reg == DS_CTRL_REG_NOT_SET){
                    // control register not set
                    writeValue(output, val);
                }
                val = Clock_Pulse();
                writeACK(val, output);
            }
        } while (val <= I2C_MAX_BYTE_VALUE);
    }
}
