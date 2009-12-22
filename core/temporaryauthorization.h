/*
 * This file is part of the PolKit1-qt project
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

#ifndef TEMPORARYAUTHORIZATION_H
#define TEMPORARYAUTHORIZATION_H

#include "subject.h"

#include <QtCore/QObject>
#include <QtCore/QDateTime>

typedef struct _PolkitTemporaryAuthorization PolkitTemporaryAuthorization;

/**
 * \namespace PolkitQt1 PolkitQt
 *
 * \brief Namespace wrapping PolicyKit-Qt classes
 *
 * This namespace wraps all PolicyKit-Qt classes.
 */
namespace PolkitQt1
{

/**
 * \class TemporaryAuthorization temporaryauthorization.h TemporaryAuthorization
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * \brief This class represents PolicyKit temporary authorization
 *
 * This class encapsulates the PolkitTemporaryAuthorization interface.
 */
class POLKITQT1_EXPORT TemporaryAuthorization : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates TemporaryAuthorization object from PolkitTemporaryAuthorization
     *
     * \warning It shouldn't be used directly unless you are completely aware of what are you doing
     *
     * \param pkTemporaryAuthorization PolkitTemporaryAuthorization object
     * \param parent
     */
    explicit TemporaryAuthorization(PolkitTemporaryAuthorization *pkTemporaryAuthorization, QObject *parent = 0);

    ~TemporaryAuthorization();

    /**
     * \brief Gets the identifier for the authorization.
     *
     * This identifier can be user by the revokeTemporaryAuthorization function
     *
     * \return Unique identifier for the authorization
     */
    QString id() const;

    /**
     * \brief Gets the identifier of the action that authorization is for
     *
     * \return String that identifies the action
     */
    QString actionId() const;

    /**
     * \brief Gets the subject that authorization is for
     *
     * \return A Subject.
     */
    Subject *subject();

    /**
     * \brief Gets the time when authorization was obtained
     *
     * \return Time of obtaining the authorization
     */
    QDateTime obtainedAt() const;

    /**
     * \brief Gets the time when authorizaton will expire
     *
     * \return Time of expiration
     */
    QDateTime expirationTime() const;

    /**
     * \brief Revoke temporary authorization
     *
     * \return \c true Authorization has been revoked
     *         \c false Revoking authorization failed
     */
    bool revoke();

private:
    class Private;
    Private * const d;
};
}

#endif // TEMPORARYAUTHORIZATION_H
