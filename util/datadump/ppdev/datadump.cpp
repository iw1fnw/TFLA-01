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
#include <vector>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/ppdev.h>
#include <linux/parport.h>

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
    char devicefile[PATH_MAX];

    std::snprintf(devicefile, PATH_MAX, "/dev/parport%d", port_number);
    int fd = open(devicefile, O_RDWR);
    if (fd < 0) {
        std::cerr << "Unable to open '" << devicefile << "': " << std::strerror(errno) << std::endl;
        return false;
    }

    // claim
    int ret = ioctl(fd, PPCLAIM);
    if (ret != 0) {
        std::cerr << "Failed to run ioctl(PPCLAIM), error=" << std::strerror(errno) << std::endl;
        close(fd);
        return false;
    }

    // direction set
    int direction = 1; // reverse direction
    ret = ioctl(fd, PPDATADIR, &direction);
    if (ret != 0) {
        std::cerr << "Failed to run ioctl(PPDATADIR), error=" << std::strerror(errno) << std::endl;
        close(fd);
        return false;
    }

    time_t start = std::time(NULL);
    time_t last = start;
    time_t now = std::time(NULL);
    while (std::difftime(now, start) < seconds) {
        unsigned char data;
        ret = ioctl(fd, PPRDATA, &data);
        if (ret != 0) {
            std::cerr << "Failed to run ioctl(PPRDATA), error=" << std::strerror(errno) << std::endl;
            close(fd);
            return false;
        }
        result.push_back(data);

        // print a progress indicator every second
        if (last != now)
            std::cerr << "." << std::flush;
        last = now;
        now = std::time(NULL);
    }
    std::cerr << "." << std::endl;

    ioctl(fd, PPRELEASE);
    close(fd);

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
