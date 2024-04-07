# (c) 2008-2010, Bernhard Walle <bernhard@bwalle.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

project(tfla-01 C CXX)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)


include(FindQt4)
set(QT_USE_QTMAIN     1) # only Windows
set(QT_USE_QTNETWORK  1) # for the update check
include(${QT_USE_FILE})

# ieee1284
include(Findieee1284)
include_directories(${IEEE1284_INCLUDE_DIR})

if (IEEE1284_FOUND)
    set(HAVE_LIBIEEE1284 1)
else (IEEE1284_FOUND)
    set(HAVE_LIBIEEE1284 0)
endif (IEEE1284_FOUND)

if (NOT IEEE1284_FOUND)
    message("libieee1284 not found. Compiling only a viewer for saved files.")
endif (NOT IEEE1284_FOUND)

configure_file(
    "${PROJECT_SOURCE_DIR}/tfla01config.h.in"
    "${PROJECT_BINARY_DIR}/tfla01config.h"
)

# include the current working directory
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR})

# read the version from a file
file(STRINGS "VERSION" VERSION_STRING)

# set the version as preprocessor defines
add_definitions(-DVERSION_STRING="${VERSION_STRING}")

# always print compiler warnings
IF (CMAKE_HOST_UNIX)
    ADD_DEFINITIONS(-Wall -W -Wno-unused-parameter)
ENDIF (CMAKE_HOST_UNIX)

IF (MSVC)
    # MSVC++ ignores all exception specifications except '()' [i.e. no exception]
    # we can safely ignore that warning when compiling on MS Windows
    ADD_DEFINITIONS("/w44290")
ENDIF (MSVC)

add_subdirectory(src)
add_subdirectory(doc)
if (BUILD_UTILS)
    add_subdirectory(util)
endif (BUILD_UTILS)


#
# Installation
#

install(
    FILES
        ${CMAKE_SOURCE_DIR}/images/tfla-01_32.png
    DESTINATION
        share/pixmaps/
    RENAME
        tfla-01.png
)

# :tabSize=4:noTabs=true:indentSize=4:mode=cmake:folding=explicit:
# vim: set sw=4 ts=4 et fdm=marker:
