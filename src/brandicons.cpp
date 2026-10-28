/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "brandicons.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QString>

namespace {

template <typename DrawFunction>
QPixmap makeBadge(int size, const QColor &color, DrawFunction draw)
{
    const int logicalSize = qMax(size, 12);
    constexpr int scale = 2;

    QPixmap canvas(logicalSize * scale, logicalSize * scale);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.scale(scale, scale);

    const QRectF rect(0.5, 0.5, logicalSize - 1.0, logicalSize - 1.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect, logicalSize * 0.28, logicalSize * 0.28);

    draw(painter, qreal(logicalSize));
    painter.end();
    return canvas.scaled(logicalSize, logicalSize,
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

} // namespace

QPixmap BrandIcons::qt(int size)
{
    return makeBadge(size, QColor(0x41, 0xCD, 0x52), [](QPainter &painter, qreal s) {
        QFont font = painter.font();
        font.setBold(true);
        font.setPixelSize(qMax(7, qRound(s * 0.42)));
        painter.setFont(font);
        painter.setPen(Qt::white);
        painter.drawText(QRectF(0.0, 0.0, s, s), Qt::AlignCenter, QStringLiteral("Qt"));
    });
}

QPixmap BrandIcons::llamaFactory(int size)
{
    return makeBadge(size, QColor(0xF5, 0x9E, 0x0B), [](QPainter &painter, qreal s) {
        QPainterPath factory;
        factory.moveTo(0.18 * s, 0.78 * s);
        factory.lineTo(0.18 * s, 0.52 * s);
        factory.lineTo(0.36 * s, 0.60 * s);
        factory.lineTo(0.36 * s, 0.36 * s);
        factory.lineTo(0.46 * s, 0.36 * s);
        factory.lineTo(0.46 * s, 0.56 * s);
        factory.lineTo(0.62 * s, 0.46 * s);
        factory.lineTo(0.62 * s, 0.30 * s);
        factory.lineTo(0.72 * s, 0.30 * s);
        factory.lineTo(0.72 * s, 0.50 * s);
        factory.lineTo(0.84 * s, 0.44 * s);
        factory.lineTo(0.84 * s, 0.78 * s);
        factory.closeSubpath();

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawPath(factory);

        painter.setBrush(QColor(0xF5, 0x9E, 0x0B));
        painter.drawRect(QRectF(0.28 * s, 0.64 * s, 0.06 * s, 0.07 * s));
        painter.drawRect(QRectF(0.50 * s, 0.62 * s, 0.06 * s, 0.07 * s));
        painter.drawRect(QRectF(0.66 * s, 0.64 * s, 0.06 * s, 0.07 * s));
    });
}

QPixmap BrandIcons::deepSeek(int size)
{
    return makeBadge(size, QColor(0x4D, 0x6B, 0xFE), [](QPainter &painter, qreal s) {
        QPainterPath whale;
        whale.moveTo(0.16 * s, 0.58 * s);
        whale.cubicTo(0.20 * s, 0.40 * s, 0.42 * s, 0.34 * s, 0.58 * s, 0.44 * s);
        whale.lineTo(0.78 * s, 0.34 * s);
        whale.lineTo(0.74 * s, 0.50 * s);
        whale.lineTo(0.86 * s, 0.66 * s);
        whale.lineTo(0.64 * s, 0.60 * s);
        whale.cubicTo(0.56 * s, 0.72 * s, 0.28 * s, 0.72 * s, 0.16 * s, 0.58 * s);
        whale.closeSubpath();

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawPath(whale);

        painter.setBrush(QColor(0x4D, 0x6B, 0xFE));
        painter.drawEllipse(QPointF(0.32 * s, 0.53 * s), 0.035 * s, 0.035 * s);

        painter.setPen(QPen(Qt::white, qMax<qreal>(1.0, 0.05 * s),
                            Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPointF(0.30 * s, 0.34 * s), QPointF(0.28 * s, 0.22 * s));
        painter.drawLine(QPointF(0.37 * s, 0.33 * s), QPointF(0.39 * s, 0.21 * s));
    });
}
