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
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>

#include "analyzerresultdialog.h"
#include "settings.h"

// -------------------------------------------------------------------------------------------------
AnalyzerResultDialog::AnalyzerResultDialog(const QString &protocol, QWidget *parent)
    throw ()
    : QDialog(parent)
{
    setWindowTitle(tr("%1 analyze result").arg(protocol));
    setupUI();
}

// -------------------------------------------------------------------------------------------------
void AnalyzerResultDialog::setResult(const QString &text)
    throw ()
{
    m_resultText->setPlainText(text);
}

// -------------------------------------------------------------------------------------------------
void AnalyzerResultDialog::setupUI()
    throw ()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton *saveButton = new QPushButton(tr("&Save to file..."), this);
    buttonBox->addButton(saveButton, QDialogButtonBox::AcceptRole);
    QPushButton *closeButton = buttonBox->addButton(QDialogButtonBox::Close);

    m_resultText = new QTextEdit(this);
    m_resultText->setReadOnly(true);
    m_resultText->setWordWrapMode(QTextOption::WordWrap);
    m_resultText->setLineWrapMode(QTextEdit::FixedColumnWidth);
    m_resultText->setLineWrapColumnOrWidth(100);

    QFont monoFont("Monospaced");
    monoFont.setStyleHint(QFont::TypeWriter);
    m_resultText->setFont(monoFont);

    verticalLayout->addWidget(m_resultText);
    verticalLayout->addWidget(buttonBox);

    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    resize(
        Settings::set().readNumEntry("AnalyzerResultDialog/width", 500),
        Settings::set().readNumEntry("AnalyzerResultDialog/height", 500)
    );
}

// -------------------------------------------------------------------------------------------------
void AnalyzerResultDialog::resizeEvent(QResizeEvent *evt)
    throw ()
{
    QDialog::resizeEvent(evt);
    Settings::set().writeEntry("AnalyzerResultDialog/width", width());
    Settings::set().writeEntry("AnalyzerResultDialog/height", height());
}

// -------------------------------------------------------------------------------------------------
void AnalyzerResultDialog::save()
    throw ()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Choose file name ..."),
                                                    QDir::currentPath(),
                                                    tr("Text files (*.txt)"));
    if (filename.isNull())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << m_resultText->toPlainText();

    file.close();
}

// vim: set sw=4 ts=4 tw=100:
