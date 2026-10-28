/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QString>

namespace Theme {
enum Mode {
    Light = 0,
    Dark = 1
};

QString styleSheet(Mode mode);
}

/*
 * Owns the current color scheme, applies the application-wide stylesheet and
 * palette, and remembers the user's choice through QSettings.
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager &instance();

    Theme::Mode mode() const;
    bool isDark() const;

    void setMode(Theme::Mode mode);
    void toggle();
    void apply() const;

signals:
    void modeChanged();

private:
    explicit ThemeManager(QObject *parent = nullptr);

    Theme::Mode m_mode;
};

#endif // THEME_H
