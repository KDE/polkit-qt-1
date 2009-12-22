/*
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 * Copyright (C) 2009 Dario Freddi <drf@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _POLKITQT1_VERSION_H_
#define _POLKITQT1_VERSION_H_

#include "export.h"

/// @brief PolkitQt version as string at compile time.
#define POLKITQT1_VERSION_STRING "${CMAKE_POLKITQT-1_VERSION_STRING}"

/// @brief The major PolkitQt version number at compile time
#define POLKITQT1_VERSION_MAJOR ${CMAKE_MAJOR_POLKITQT-1_VERSION}

/// @brief The minor PolkitQt version number at compile time
#define POLKITQT1_VERSION_MINOR ${CMAKE_MINOR_POLKITQT-1_VERSION}

/// @brief The PolkitQt release version number at compile time
#define POLKITQT1_VERSION_RELEASE ${CMAKE_RELEASE_POLKITQT-1_VERSION}

/**
 * \brief Create a unique number from the major, minor and release number of a %PolkitQt version
 *
 * This function can be used for preprocessing. For version information at runtime
 * use the version methods in the PolkitQt namespace.
 */
#define POLKITQT1_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %PolkitQt Version as a unique number at compile time
 *
 * This macro calculates the %PolkitQt version into a number. It is mainly used
 * through POLKITQT1_IS_VERSION in preprocessing. For version information at runtime
 * use the version methods in the PolkitQt namespace.
 */
#define POLKITQT1_VERSION \
    POLKITQT1_MAKE_VERSION(POLKITQT1_VERSION_MAJOR,POLKITQT1_VERSION_MINOR,POLKITQT1_VERSION_RELEASE)

/**
 * \brief Check if the %PolkitQt version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if POLKITQT1_IS_VERSION(2,1)
 * // Code for PolkitQt 2.1
 * #else
 * // Code for PolkitQt 2.0
 * #endif
 * \endcode
 *
 * For version information at runtime
 * use the version methods in the PolkitQt namespace.
 */
#define POLKITQT1_IS_VERSION(a,b,c) ( POLKITQT1_VERSION >= POLKITQT1_MAKE_VERSION(a,b,c) )


namespace PolkitQt1 {
    /**
     * @brief Returns the major number of PolkitQt's version, e.g.
     * 1 for %PolkitQt 1.0.2.
     * @return the major version number at runtime.
     */
    POLKITQT1_EXPORT unsigned int versionMajor();

    /**
     * @brief Returns the minor number of PolkitQt's version, e.g.
     * 0 for %PolkitQt 1.0.2.
     * @return the minor version number at runtime.
     */
    POLKITQT1_EXPORT unsigned int versionMinor();

    /**
     * @brief Returns the release of PolkitQt's version, e.g.
     * 2 for %PolkitQt 1.0.2.
     * @return the release number at runtime.
     */
    POLKITQT1_EXPORT unsigned int versionRelease();

    /**
     * @brief Returns the %PolkitQt version as string, e.g. "1.0.2".
     *
     * On contrary to the macro POLKITQT1_VERSION_STRING this function returns
     * the version number of PolkitQt at runtime.
     * @return the %PolkitQt version. You can keep the string forever
     */
    POLKITQT1_EXPORT const char* versionString();
}

#endif
