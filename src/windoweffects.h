/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef WINDOWEFFECTS_H
#define WINDOWEFFECTS_H

class QWidget;

namespace WindowEffects {

/*
 * Applies the native backdrop / title-bar appearance for the given window.
 *
 * On Windows this enables an acrylic (frosted glass) blur behind the window,
 * the immersive dark title bar, and rounded window corners where supported.
 * On other platforms it is a no-op; the translucent glass styling still works
 * through the application stylesheet.
 */
void apply(QWidget *window, bool dark);

}

#endif // WINDOWEFFECTS_H
