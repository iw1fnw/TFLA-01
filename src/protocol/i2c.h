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
#ifndef I2C_H
#define I2C_H

#include "analyzer.h"
#include "data.h"

class I2cAnalyzer : public ProtocolAnalyzer
{
    public:
        QString getId() const throw ();
        QString getName() const throw ();
        QStringList getChannels() const throw ();

        void analyze(const Data &data, int start, int end, QTextStream &output)
        throw (TfError);

    protected:
        void setI2C_Masks() throw ();
        void setSize(int end) throw ();
        int  getByte() throw ();
        int  getStart() throw ();
        int  Clock_Pulse() throw ();
        void writeCmd(QTextStream &output, QString text) throw ();
        void writeValue(QTextStream &output, int val) throw ();
        void writeError(int val, QTextStream &output) throw();
        void writeACK(int val, QTextStream &output) throw();
        void processUnknowIC(int val, QTextStream &output) throw ();
        void processPCF8574(int val, QTextStream &output) throw ();
        void processRTC(int val, QTextStream &output) throw ();
        void Decode_RTC(QTextStream &output, int val) throw ();

    private:
        unsigned char  m_SCL_Mask;
        unsigned char  m_SDA_Mask;
        unsigned char  m_I2C_Mask;
        Data           m_currentData;
        int            m_currentPosition;
        int            m_size;
        int            ds_cont_reg;
};

#endif /* I2C_H */
