/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "mainwindow.h"
#include "appversion.h"
#include "theme.h"

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("HandsomeTurtle0307"));
    QApplication::setApplicationName(QStringLiteral("OllaMerge"));
    QApplication::setApplicationVersion(appVersionString());

    if (QStyle *fusion = QStyleFactory::create(QStringLiteral("Fusion"))) {
        QApplication::setStyle(fusion);
    }
    ThemeManager::instance().apply();

    MainWindow w;
    w.show();
    return QApplication::exec();
}
