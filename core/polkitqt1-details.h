/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Radek Novacek <rnovacek@redhat.com>
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

#ifndef POLKITQT1_DETAILS_H
#define POLKITQT1_DETAILS_H

#include "polkitqt1-export.h"

#include <QtCore/QObject>
#include <QtCore/QSharedData>

typedef struct _PolkitDetails PolkitDetails;

/**
 * \namespace PolkitQt1 PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt1
{


/**
 * \class Details polkitqt1-details.h Details
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * \brief Class used for passing details around.
 */
class POLKITQT1_EXPORT Details
{
public:
    /**
     * Creates a new Details object
     */
    Details();

    /**
     * Creates Details object from PolkitDetails
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkDetails PolkitDetails object
     */
    explicit Details(PolkitDetails *pkDetails);

    ~Details();

    Details &operator=(const Details &other);

    /**
     * Get the value for \p key
     *
     * \param key A key
     * \return Value of the key \p key, otherwise empty QString.
     */
    QString lookup(const QString &key) const;

    /**
     * Inserts key \p key with value \p value.
     *
     * \param key A key.
     * \param value A value.
     */
    void insert(const QString &key, const QString &value);

    /**
     * Gets a list of all keys.
     *
     * \return List of all keys.
     */
    QStringList keys() const;
private:
    class Data;
    QExplicitlySharedDataPointer< Data > d;
};

}

#endif // DETAILS_H
