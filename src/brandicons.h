/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef BRANDICONS_H
#define BRANDICONS_H

#include <QPixmap>

namespace BrandIcons {

// Small rounded badges drawn in code, so the About dialog needs no image
// assets or SVG plugin.
QPixmap qt(int size);
QPixmap llamaFactory(int size);
QPixmap deepSeek(int size);

}

#endif // BRANDICONS_H
