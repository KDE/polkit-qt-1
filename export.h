/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
 * Copyright (C) 2007 David Faure <faure@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef POLKIT_QT_EXPORT_H
#define POLKIT_QT_EXPORT_H

/** \file export.h
    \brief Contains Macros for exporting symbols

    This file contains macros needed for exporting/importing symbols
*/

#include <QtCore/QtGlobal>

#ifndef POLKIT_QT_EXPORT
# if defined(MAKE_POLKIT_QT_LIB)
/* We are building this library */
#  define POLKIT_QT_EXPORT Q_DECL_EXPORT
# else
/* We are using this library */
#  define POLKIT_QT_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif /*POLKIT_QT_EXPORT_H*/
