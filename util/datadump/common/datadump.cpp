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
#include <vector>
#include <iostream>
#include <iomanip>

// -------------------------------------------------------------------------------------------------
void print_binary(const std::vector<unsigned char> &result, std::ostream &os)
{
    for (std::vector<unsigned char>::const_iterator it = result.begin(); it != result.end(); ++it) {
        unsigned char byte = *it;
        for (int position = 7; position >= 0; --position)
            if (byte & (1 << position))
                os <<  "1";
            else
                os <<  "0";
        os << " (0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(byte) << ")" << std::endl;
    }

    os << std::endl;
    os << std::dec << result.size() << " samples" << std::endl;
}

