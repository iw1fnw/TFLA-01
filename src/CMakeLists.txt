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

set(tfla01_SRCS
    main.cpp
    triggerwidget.cpp
    settings.cpp
    settings.h
    centralwidget.cpp
    datacollector.cpp
    datacollector.h
    parameterbox.cpp
    dataplot.cpp
    aboutdialog.cpp
    help.cpp
    dataview.cpp
    data.cpp
    data.h
    tfla01.cpp
    wheelscrollbar.cpp
    protocolsettingsdialog.cpp
    analyzerresultdialog.cpp
    updatechecker.cpp
)

if (IEEE1284_FOUND)
    include(hardware/CMakeLists.txt)
    set(tfla01_SRCS ${tfla01_SRCS} ${parport_SRCS})
endif ()

# sets protocol_SRCS
include(protocol/CMakeLists.txt)
set(tfla01_SRCS ${tfla01_SRCS} ${protocol_SRCS})

set(tfla01_MOCS
    triggerwidget.h
    centralwidget.h
    parameterbox.h
    dataplot.h
    aboutdialog.h
    help.h
    dataview.h
    tfla01.h
    wheelscrollbar.h
    protocolsettingsdialog.h
    analyzerresultdialog.h
    updatechecker.h
)

# compiler for rc files
if (MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  enable_language(RC)
  set(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> --output-format=coff <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
endif (MINGW)

# build the util programs?
option (BUILD_UTILS "Build test programs (only useful for development)")

# translations
option (UPDATE_TRANSLATIONS "Update source translation *.ts files")
if (UPDATE_TRANSLATIONS)
  QT5_CREATE_TRANSLATION(
    tfla01_qmfile
    ${tfla01_SRCS} ${tfla01_MOC_SRCS}
    ${PROJECT_SOURCE_DIR}/share/tfla-01/translations/de.ts
    )
else (UPDATE_TRANSLATIONS)
  QT5_ADD_TRANSLATION(
    tfla01_qmfile
    ${PROJECT_SOURCE_DIR}/share/tfla-01/translations/de.ts
    )
endif (UPDATE_TRANSLATIONS)

# Compiled-in pixmaps
set(QtApp_RCCS ${PROJECT_SOURCE_DIR}/tfla-01.qrc)

# generate rules for building source files from the resources
QT5_ADD_RESOURCES(tfla01_RCC_SRCS ${PROJECT_SOURCE_DIR}/tfla-01.qrc)

# generate rules for building source files that moc generates
QT5_WRAP_CPP(tfla01_MOC_SRCS ${tfla01_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(tfla-01 WIN32
    ${tfla01_SRCS} ${tfla01_MOC_SRCS} ${tfla01_RCC_SRCS} ${tfla01_qmfile}
)

target_link_libraries(tfla-01 Qt5::Widgets Qt5::Network ${IEEE1284_LIBRARIES})

#
# Installation
#

install(
    TARGETS
        tfla-01
    DESTINATION
        bin
)

install(
    FILES
        ${tfla01_qmfile}
    DESTINATION
        share/tfla-01/translations
)

# :tabSize=4:noTabs=true:indentSize=4:mode=cmake:folding=explicit:
# vim: set sw=4 ts=4 et fdm=marker:
