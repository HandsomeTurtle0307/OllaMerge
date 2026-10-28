/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "appicon.h"

#include <QColor>
#include <QLinearGradient>
#include <QPainter>
#include <QPen>
#include <QRectF>

QPixmap AppIcon::create(int size, bool dark)
{
    size = qMax(size, 16);

    // Render at 2x for smooth edges, then scale down to the requested size.
    // Keep the result at 1x device pixels: QLabel does not compensate for a
    // QPixmap devicePixelRatio here, which used to clip the mark to one quarter.
    constexpr int scale = 2;
    QPixmap canvas(size * scale, size * scale);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.scale(scale, scale);

    const QRectF rect(0.5, 0.5, size - 1.0, size - 1.0);
    const qreal radius = size * 0.26;

    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    if (dark) {
        gradient.setColorAt(0.0, QColor(0x34, 0xd3, 0x99));
        gradient.setColorAt(1.0, QColor(0x2d, 0xd4, 0xbf));
    } else {
        gradient.setColorAt(0.0, QColor(0x05, 0x96, 0x69));
        gradient.setColorAt(1.0, QColor(0x0f, 0x76, 0x6e));
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(rect, radius, radius);

    // Soft top sheen for a little depth.
    QLinearGradient sheen(rect.topLeft(), rect.bottomLeft());
    sheen.setColorAt(0.0, QColor(255, 255, 255, dark ? 34 : 52));
    sheen.setColorAt(0.55, QColor(255, 255, 255, 0));
    painter.setBrush(sheen);
    painter.drawRoundedRect(rect.adjusted(1.0, 1.0, -1.0, -1.0),
                            radius - 1.0, radius - 1.0);

    // Two overlapping rings: the merge mark.
    const QColor mark = dark ? QColor(0x05, 0x2e, 0x16) : QColor(0xff, 0xff, 0xff);
    const qreal penWidth = qMax<qreal>(1.6, size * 0.062);
    const qreal ringRadius = size * 0.205;
    const QPointF leftCenter(size * 0.375, size * 0.52);
    const QPointF rightCenter(size * 0.625, size * 0.52);

    QColor firstRing = mark;
    firstRing.setAlpha(235);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(firstRing, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(leftCenter, ringRadius, ringRadius);

    QColor secondRing = mark;
    secondRing.setAlpha(165);
    painter.setPen(QPen(secondRing, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawEllipse(rightCenter, ringRadius, ringRadius);

    // Thin inner rim to separate the tile from the window background.
    const QColor rim = dark ? QColor(255, 255, 255, 42) : QColor(255, 255, 255, 62);
    painter.setPen(QPen(rim, 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

    painter.end();
    return canvas.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
