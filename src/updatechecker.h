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
#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkReply>

class UpdateChecker : public QObject
{
    Q_OBJECT

    public:
        UpdateChecker(QWidget *parent) noexcept;

    public slots:
        void checkForUpdate() noexcept;
        void replyFinished(QNetworkReply *reply) noexcept;

    public:
        static int versionCompare(const QString &a, const QString &b) noexcept;
};

#endif /* UPDATECHECKER_H */

// vim: set sw=4 ts=4 tw=100:
