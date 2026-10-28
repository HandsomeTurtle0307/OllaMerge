/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QHash>
#include <QObject>
#include <QString>

#include <utility>

/*
 * Lightweight runtime translator.
 *
 * All UI strings are looked up by a stable key, so the interface can switch
 * between Simplified Chinese and English instantly, without restarting and
 * without depending on Qt Linguist (.ts/.qm) build tooling.
 */
class I18n : public QObject
{
    Q_OBJECT

public:
    enum Language {
        Chinese = 0,
        English = 1
    };
    Q_ENUM(Language)

    static I18n &instance();

    Language language() const;
    void setLanguage(Language language);

    QString text(const QString &key) const;
    QString text(const char *key) const;

    static QString t(const QString &key);
    static QString t(const char *key);

signals:
    void languageChanged();

private:
    explicit I18n(QObject *parent = nullptr);

    void loadSettings();

    Language m_language;
    QHash<QString, std::pair<QString, QString>> m_translations; // key -> (zh, en)
};

#endif // LANGUAGE_H
