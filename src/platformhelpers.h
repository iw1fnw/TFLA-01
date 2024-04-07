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
#ifndef PLATFORMHELPERS_H
#define PLATFORMHELPERS_H

#ifdef Q_WS_X11
#  define RUNNING_ON_X11 1
#else
#  define RUNNING_ON_X11 0
#endif

#ifdef Q_WS_MAC
#  define RUNNING_ON_MAC 1
#else
#  define RUNNING_ON_MAC 0
#endif

#if defined(Q_WS_MAC) || defined(Q_WS_X11)
#  define RUNNING_ON_POSIX 1
#else
#  define RUNNING_ON_POSIX 0
#endif

#ifdef Q_WS_WIN
#  define RUNNING_ON_WINDOWS 1
#else
#  define RUNNING_ON_WINDOWS 0
#endif

#ifdef Q_WS_QWS
#  define RUNNING_ON_EMBEDDED_LINUX 1
#else
#  define RUNNING_ON_EMBEDDED_LINUX 0
#endif

#endif // PLATFORMHELPERS_H

// vim: set sw=4 ts=4 et:
