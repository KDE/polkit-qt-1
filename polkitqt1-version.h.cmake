/*
    SPDX-FileCopyrightText: 2007 Sebastian Trueg <trueg@kde.org>
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _POLKITQT1_VERSION_H_
#define _POLKITQT1_VERSION_H_

#include "polkitqt1-core-export.h"

/// @brief PolkitQt-1 version as string at compile time.
#define POLKITQT1_VERSION_STRING "${POLKITQT-1_VERSION_STRING}"

/// @brief The major PolkitQt-1 version number at compile time
#define POLKITQT1_VERSION_MAJOR ${POLKITQT-1_VERSION_MAJOR}

/// @brief The minor PolkitQt-1 version number at compile time
#define POLKITQT1_VERSION_MINOR ${POLKITQT-1_VERSION_MINOR}

/// @brief The PolkitQt-1 patch version number at compile time
#define POLKITQT1_VERSION_PATCH ${POLKITQT-1_VERSION_PATCH}

/**
 * \brief Create a unique number from the major, minor and release number of a %PolkitQt-1 version
 *
 * This function can be used for preprocessing. For version information at runtime
 * use the version methods in the PolkitQt-1 namespace.
 */
#define POLKITQT1_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %PolkitQt-1 Version as a unique number at compile time
 *
 * This macro calculates the %PolkitQt-1 version into a number. It is mainly used
 * through POLKITQT1_IS_VERSION in preprocessing. For version information at runtime
 * use the version methods in the PolkitQt-1 namespace.
 */
#define POLKITQT1_VERSION \
    POLKITQT1_MAKE_VERSION(POLKITQT1_VERSION_MAJOR,POLKITQT1_VERSION_MINOR,POLKITQT1_VERSION_PATCH)

/**
 * \brief Check if the %PolkitQt-1 version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if POLKITQT1_IS_VERSION(2,1)
 * // Code for PolkitQt-1 2.1
 * #else
 * // Code for PolkitQt-1 2.0
 * #endif
 * \endcode
 *
 * For version information at runtime
 * use the version methods in the PolkitQt-1 namespace.
 */
#define POLKITQT1_IS_VERSION(a,b,c) ( POLKITQT1_VERSION >= POLKITQT1_MAKE_VERSION(a,b,c) )


namespace PolkitQt1 {
    /**
     * @brief Returns the major number of PolkitQt-1's version, e.g.
     * 1 for %PolkitQt-1 1.0.2.
     * @return the major version number at runtime.
     */
    POLKITQT1_CORE_EXPORT unsigned int versionMajor();

    /**
     * @brief Returns the minor number of PolkitQt-1's version, e.g.
     * 0 for %PolkitQt-1 1.0.2.
     * @return the minor version number at runtime.
     */
    POLKITQT1_CORE_EXPORT unsigned int versionMinor();

    /**
     * @brief Returns the patch number of PolkitQt-1's version, e.g.
     * 2 for %PolkitQt-1 1.0.2.
     * @return the release number at runtime.
     */
    POLKITQT1_CORE_EXPORT unsigned int versionPatch();

    /**
     * @brief Returns the %PolkitQt-1 version as string, e.g. "1.0.2".
     *
     * On contrary to the macro POLKITQT1_VERSION_STRING this function returns
     * the version number of PolkitQt-1 at runtime.
     * @return the %PolkitQt-1 version. You can keep the string forever
     */
    POLKITQT1_CORE_EXPORT const char* versionString();
}

#endif
