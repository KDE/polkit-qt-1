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

#ifndef DETAILS_H
#define DETAILS_H

#include "export.h"

#include <QtCore/QObject>

typedef struct _PolkitDetails PolkitDetails;

/**
 * \namespace PolkitQt PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt
{

/**
 * \class Details details.h Details
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * \brief Class used for passing details around.
 */
class POLKITQT1_EXPORT Details : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new Details object
     * \param parent
     */
    Details(QObject *parent = 0);

    /**
     * Creates Details object from PolkitDetails
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkDetails PolkitDetails object
     * \param parent
     */
    explicit Details(PolkitDetails *pkDetails, QObject *parent = 0);

    ~Details();

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
    QStringList getKeys() const;
private:
    class Private;
    Private * const d;
};

}

#endif // DETAILS_H
