/*
 * Copyright (c) 2005-2009, Bernhard Walle
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
#ifndef TFERROR_H
#define TFERROR_H

#include <QString>
#include <QObject>

/**
 * Exception base class (or interface) for this application. A common base class for all exception
 * simplifies catching them since all contains a method for a properly formatted error message.
 * Using  not std::exception has the advantage of using QString and internationalized error message.
 *
 * Since this application don't use other C++ libraries that use std::exception, there's no
 * disadvantage.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 */
class TfError
{
    public:

        /**
         * Returns a translated error message, well formatted for presentation to ther user.
         */
        virtual QString what() const = 0;

        /**
         * Destructor.
         */
        virtual ~TfError() {}
};

#endif /* TFERROR_H */

// vim: set sw=4 ts=4 tw=100:
