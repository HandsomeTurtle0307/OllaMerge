/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef APPVERSION_H
#define APPVERSION_H

#include <QString>

#ifndef OLLAMERGE_VERSION
#define OLLAMERGE_VERSION "3.0"
#endif

inline QString appVersionString()
{
    return QString::fromLatin1(OLLAMERGE_VERSION);
}

#endif // APPVERSION_H
