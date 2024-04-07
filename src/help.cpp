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
#include <memory>

#include <QObject>
#include <QApplication>
#include <QMessageBox>
#include <QWidget>
#include <QTextCodec>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QLocale>

#include "global.h"
#include "tfla01.h"
#include "help.h"
#include "settings.h"
#include "aboutdialog.h"


// -------------------------------------------------------------------------------------------------
void Help::showAbout()
{
    std::auto_ptr<AboutDialog> dlg(new AboutDialog(Tfla01::instance()));

    dlg->exec();
}


// -------------------------------------------------------------------------------------------------
void Help::showHelp()
{
    QString base;
    if (QDir(qApp->applicationDirPath() + "/doc/user/").exists())
        base = QDir(qApp->applicationDirPath() + "/doc/user/").canonicalPath();
    else
        base = QDir(qApp->applicationDirPath() + "/../share/tfla-01/doc/").canonicalPath();
    QString loc = QString(QLocale::system().name()).section("_", 0, 0);
    QStringList args;

    if (QFile::exists(base + "/" + loc + "/index.html")) {
		QString url = "file:///" + base + "/" + loc + "/index.html";
		QDesktopServices::openUrl(QUrl(url));
    } else if (QFile::exists(base + "/en/index.html")) {
		QString url = "file:///" + base + "/en/index.html";
		QDesktopServices::openUrl(QUrl(url));
    } else {
        QMessageBox::critical(Tfla01::instance(), tr("TFLA-01"),
            tr("The TFLA-01 documentation is not installed."), QMessageBox::Ok,
            QMessageBox::NoButton );
    }
}

// vim: set sw=4 ts=4 tw=100:
