/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QGraphicsOpacityEffect;
class QShowEvent;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private:
    QGraphicsOpacityEffect *m_headerEffect = nullptr;
};

#endif // ABOUTDIALOG_H
