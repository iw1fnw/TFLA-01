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
#ifndef SPI_H
#define SPI_H

#include "analyzer.h"
#include "data.h"

class SpiAnalyzer : public ProtocolAnalyzer
{
    public:
        QString getId() const throw ();
        QString getName() const throw ();
        QStringList getChannels() const throw ();

        void analyze(const Data &data, int start, int end, QTextStream &output)
        throw (TfError);

    protected:
        int getCLKFreq() throw();
        void setSPIMasks() throw ();
        void setSPISize(int end) throw ();
        int getSDABit() throw();
        int getSPIStart() throw ();
        int clkPulse() throw ();

    private:
        QString binRepByte(unsigned char val) throw();

    private:
        unsigned char m_SCK_Mask;
        unsigned char m_SDI_SDO_Mask;
        unsigned char m_SPI_Mask;
        Data m_currentData;
        int m_currentPosition;
        int m_size;
};

#endif /* SPI_H */
