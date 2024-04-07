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
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Set the CMAKE_PREFIX_PATH to point to the Qt5 installation directory
# if it's not set by the user.
#if(NOT CMAKE_PREFIX_PATH)
#    set(CMAKE_PREFIX_PATH "/usr/lib/qt5/bin" CACHE PATH "Qt5 installation directory")
#endif()

# Qt5
find_package(Qt5 COMPONENTS Widgets Network LinguistTools REQUIRED)
if (Qt5Widgets_FOUND AND Qt5Network_FOUND AND Qt5LinguistTools_FOUND)
  # The Qt5Widgets_INCLUDES also includes the include directories for
    # dependencies QtCore and QtGui
  include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5LinguistTools_INCLUDE_DIRS})
  # We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
  add_definitions(${Qt5Widgets_DEFINITIONS} ${QtNetwork_DEFINITIONS} ${Qt5LinguistTools_DEFINITIONS})
  # Executables fail to build with Qt 5 in the default configuration
  # without -fPIE. We add that here.
  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
else ()
    message( FATAL_ERROR "QT5 not found.")
endif ()

# ieee1284
find_package(ieee1284)
if (IEEE1284_FOUND)
    include_directories(${IEEE1284_INCLUDE_DIR})
    set(HAVE_LIBIEEE1284 1)
else ()
    set(HAVE_LIBIEEE1284 0)
    message("libieee1284 not found. Compiling only a viewer for saved files.")
endif ()

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
