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
#include <QLabel>
#include <QFile>
#include <QTextEdit>
#include <QApplication>
#include <QDir>
#include <QPushButton>
#include <QPixmap>
#include <QBoxLayout>
#include <QTextStream>
#include <QFont>

#include "aboutdialog.h"

// -------------------------------------------------------------------------------------------------
AboutDialog::AboutDialog(QWidget* parent)
    throw ()
    : QDialog(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(6);
    setWindowTitle(tr("About The Fabulous Logic Analyzer"));

    // the top of the dialog
    QWidget* titleBox = new QWidget;
    QHBoxLayout *titleBoxLayout = new QHBoxLayout;

    QLabel* titleIcon = new QLabel;
    titleIcon->setPixmap(QPixmap(":/tfla-01_32.png"));
    QLabel* titleText = new QLabel(tr("<p><b>The Fabulous Logic Analyzer " VERSION_STRING "</b></p>"));

    titleBoxLayout->addWidget(titleIcon);
    titleBoxLayout->setStretchFactor(titleIcon, 0);
    titleBoxLayout->addWidget(titleText);
    titleBoxLayout->setStretchFactor(titleText, 1);
    titleBoxLayout->setSpacing(5);
    titleBox->setLayout(titleBoxLayout);

    // the tab
    m_tabWidget = new QTabWidget(this);
    setupAboutTab();
    setupLicenseTab();

    // the ok Button
    QPushButton* okButton = new QPushButton(tr("&Close"), this);
    okButton->setDefault(true);
    QWidget* filler = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(6);
    buttonLayout->addWidget(filler);
    buttonLayout->addWidget(okButton);
    buttonLayout->setStretchFactor(filler, 1);
    buttonLayout->setStretchFactor(okButton, 0);

    // main layout
    m_mainLayout->addWidget(titleBox);
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->setStretchFactor(m_tabWidget, 5);
    m_mainLayout->addLayout(buttonLayout);

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
}


// -------------------------------------------------------------------------------------------------
void AboutDialog::setupAboutTab()
{
    QWidget *aboutTab = new QWidget;
    QHBoxLayout *aboutTabLayout = new QHBoxLayout;

    QLabel *label = new QLabel(
        tr("<p>This is a logic analyzer tool written for Unix and Windows "
            "written in C++<br /> using the Qt library.</p>"
            "<p><b>Thanks to:</b>"
            "<ul><li>Trolltech for the Qt library</li>"
            "<li>Tim Waugh for the IEEE1284 (parallelport) library</li>"
            "<li>Gtk+ artists for the nice stock icons</li>"
            "</ul></p>"
            "<p><b>Homepage:</b> <a href=\"http://tfla-01.berlios.de\">http://tfla-01.berlios.de</a></p>"), aboutTab);
    label->setAlignment(Qt::AlignTop);
    label->setOpenExternalLinks(true);

    aboutTabLayout->setMargin(15);
    aboutTabLayout->addWidget(label);
    aboutTab->setLayout(aboutTabLayout);

    m_tabWidget->addTab(aboutTab, tr("&About"));
}


// -------------------------------------------------------------------------------------------------
void AboutDialog::setupLicenseTab()
{
    QWidget *licenseTab = new QWidget;
    QHBoxLayout *licenseTabLayout = new QHBoxLayout;

    QTextEdit* textEdit = new QTextEdit(licenseTab);
    textEdit->setReadOnly(true);
    textEdit->setWordWrapMode(QTextOption::WordWrap);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);

    QFont monoFont("Monospaced");
    monoFont.setStyleHint(QFont::TypeWriter);
    textEdit->setFont(monoFont);

    QFile file(":COPYING");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        textEdit->setText(stream.readAll());
    }

    licenseTabLayout->setMargin(15);
    licenseTabLayout->addWidget(textEdit);
    licenseTab->setLayout(licenseTabLayout);

    m_tabWidget->addTab(licenseTab, tr("&License"));
}

// vim: set sw=4 ts=4 tw=100:
