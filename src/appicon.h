/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef APPICON_H
#define APPICON_H

#include <QPixmap>

namespace AppIcon {

/*
 * Draws the OllaMerge mark: a rounded emerald tile with two overlapping rings
 * (base model + LoRA adapter merging into one). Light and dark modes use
 * different green gradients and foreground colors.
 */
QPixmap create(int size, bool dark);

}

#endif // APPICON_H
