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
#include <limits>

#include <QApplication>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>

#include "updatechecker.h"
#include "settings.h"

// -------------------------------------------------------------------------------------------------
UpdateChecker::UpdateChecker(QWidget *parentWidget)
    throw ()
    : QObject(parentWidget)
{}

// -------------------------------------------------------------------------------------------------
void UpdateChecker::checkForUpdate()
    throw ()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl("https://raw.githubusercontent.com/iw1fnw/TFLA-01/main/VERSION"));

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));


    manager->get(request);
}

// -------------------------------------------------------------------------------------------------
void UpdateChecker::replyFinished(QNetworkReply *reply)
    throw ()
{
    QByteArray version = reply->readAll();
    QString versionString = QString::fromUtf8(version);
    versionString = versionString.trimmed();

    if (versionCompare(versionString, VERSION_STRING) > 0 &&
        versionCompare(versionString, Settings::set().readEntry("LastReceivedVersion")) > 0)
        QMessageBox::information(static_cast<QWidget *>(parent()), tr("TFLA-01"),
                                 tr("There's an updated version of TFLA-01 available (%1). You can download "
                                    "the new version from <a href=\"http://tfla-01.berlios.de\">"
                                    "http://tfla-01.berlios.de</a>.").arg(versionString));

    Settings::set().writeEntry("LastReceivedVersion", versionString);
}

// -------------------------------------------------------------------------------------------------
int UpdateChecker::versionCompare(const QString &a, const QString &b)
    throw ()
{
    int major_a, minor_a, patch_a;
    int major_b, minor_b, patch_b;

    QStringList split = a.split(".");
    if (split.size() != 3)
        return std::numeric_limits<int>::max();

    bool ok;
    major_a = split[0].toUInt(&ok);
    if (!ok)
        return std::numeric_limits<int>::max();
    minor_a = split[1].toUInt(&ok);
    if (!ok)
        return std::numeric_limits<int>::max();
    patch_a = split[2].toUInt(&ok);
    if (!ok)
        return std::numeric_limits<int>::max();

    split = b.split(".");
    if (split.size() != 3)
        return std::numeric_limits<int>::max();

    major_b = split[0].toUInt(&ok);
    if (!ok)
        return std::numeric_limits<int>::max();
    minor_b = split[1].toUInt(&ok);
    if (!ok)
        return std::numeric_limits<int>::max();
    patch_b = split[2].toUInt(&ok);
    if (!ok)
        return std::numeric_limits<int>::max();

    if (major_a != major_b)
        return major_a - major_b;

    if (minor_a != minor_b)
        return minor_a - minor_b;

    if (patch_a != patch_b)
        return patch_a - patch_b;

    return 0;
}

// vim: set sw=4 ts=4 tw=100:
