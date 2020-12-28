/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>
    SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_EXPORT_H
#define POLKITQT1_EXPORT_H

/** \file polkitqt1-export.h
    \brief Contains Macros for exporting symbols

    This file contains macros needed for exporting/importing symbols
*/

#include <QtCore/QtGlobal>

#ifndef POLKITQT1_EXPORT
# if defined(MAKE_POLKITQT1_LIB)
/* We are building this library */
#  define POLKITQT1_EXPORT Q_DECL_EXPORT
# else
/* We are using this library */
#  define POLKITQT1_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif /*POLKITQT1_EXPORT_H*/

