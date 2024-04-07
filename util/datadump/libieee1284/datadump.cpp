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
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <vector>
#include <iomanip>

#include <ieee1284.h>

#include "../common/datadump.h"

const double DEFAULT_MEASURE_TIME = 10.0;

// -------------------------------------------------------------------------------------------------
void usage()
{
    std::cout << "Usage: datadump <portnumber>" << std::endl;
}

// -------------------------------------------------------------------------------------------------
bool read_values(int port_number, double seconds, std::vector<unsigned char> &result)
{
    struct parport_list pplist;
    int ret = ieee1284_find_ports(&pplist, 0);
    if (ret != E1284_OK) {
        std::cerr << "Unable to call ieee1284_find_ports(), error=" << ret << std::endl;
        return false;
    }

    if (port_number >= pplist.portc) {
        std::cerr << "Port number " << port_number << " is out of range, max=" << pplist.portc << std::endl;
        return false;
    }

    struct parport* port = pplist.portv[port_number];
    assert(port != NULL);

    // open
    int cap = 0;
    ret = ieee1284_open(port, 0, &cap);
    if (ret != E1284_OK) {
        std::cerr << "Failed to run ieee1284_open(), error=" << ret << std::endl;
        return false;
    }

    // claim
    ret = ieee1284_claim(port);
    if (ret != E1284_OK) {
        std::cerr << "Failed to run ieee1284_claim(), error=" << ret << std::endl;
        ieee1284_release(port);
        ieee1284_close(port);
        return false;
    }

    // direction set
    ret = ieee1284_data_dir(port, true);
    if (ret != E1284_OK) {
        std::cerr << "Failed to run ieee1284_data_dir(), error=" << ret << std::endl;
        ieee1284_release(port);
        ieee1284_close(port);
        return false;
    }

    time_t start = std::time(NULL);
    time_t last = start;
    time_t now = std::time(NULL);
    while (std::difftime(now, start) < seconds) {
        unsigned char data = ieee1284_read_data(port);
        result.push_back(data);

        // print a progress indicator every second
        if (last != now)
            std::cerr << "." << std::flush;
        last = now;
        now = std::time(NULL);
    }
    std::cerr << "." << std::endl;

    ieee1284_release(port);
    ieee1284_close(port);

    return true;
}

// -------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int portnumber;

    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }

    std::stringstream ss;
    ss << argv[1];
    ss >> portnumber;

    std::vector<unsigned char> result;
    bool success = read_values(portnumber, DEFAULT_MEASURE_TIME, result);
    if (!success)
        return EXIT_FAILURE;

    print_binary(result, std::cout);

    return EXIT_SUCCESS;
}
