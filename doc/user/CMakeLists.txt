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

find_program(XSLTPROC_EXECUTABLE xsltproc)

# create target directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc/user/en)

if (CMAKE_HOST_UNIX)
    set(XSLTPROC_ARGS "--nonet")
endif (CMAKE_HOST_UNIX)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/doc/user/en/index.html # creates more files
    COMMAND
        ${XSLTPROC_EXECUTABLE} ${XSLTPROC_ARGS}
        ${CMAKE_SOURCE_DIR}/doc/user/tfla-01.xsl
        ${CMAKE_SOURCE_DIR}/doc/user/en/tfla-01.xml &&
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/doc/user/tfla-01.css
        ${CMAKE_BINARY_DIR}/doc/user/tfla-01.css &&
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/doc/user/en/images
        ${CMAKE_BINARY_DIR}/doc/user/en/images
    DEPENDS
        ${CMAKE_SOURCE_DIR}/doc/user/en/tfla-01.xml
    WORKING_DIRECTORY
        ${CMAKE_BINARY_DIR}/doc/user/en
)

add_custom_target(
    doc
    DEPENDS
        ${CMAKE_BINARY_DIR}/doc/user/en/index.html
)

add_dependencies(
    tfla-01
    doc
)

#
# Installation
#

install(
    DIRECTORY
        ${CMAKE_BINARY_DIR}/doc/user/
    DESTINATION
        share/tfla-01/doc/
    FILES_MATCHING REGEX ".*.(html|css)"
)

# :tabSize=4:noTabs=true:indentSize=4:mode=cmake:folding=explicit:
# vim: set sw=4 ts=4 et fdm=marker:
