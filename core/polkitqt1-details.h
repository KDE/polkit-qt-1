/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_DETAILS_H
#define POLKITQT1_DETAILS_H

#include "polkitqt1-core-export.h"

#include <QObject>
#include <QSharedData>

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
class POLKITQT1_CORE_EXPORT Details
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

    /**
     * Copy constructor.
     */
    Details(const Details &other);

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
