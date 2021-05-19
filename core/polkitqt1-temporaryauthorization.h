/*
    This file is part of the PolKit1-qt project
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_TEMPORARYAUTHORIZATION_H
#define POLKITQT1_TEMPORARYAUTHORIZATION_H

#include "polkitqt1-subject.h"

#include <QObject>
#include <QDateTime>
#include <QMetaType>
#include <QSharedData>

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
 * \class TemporaryAuthorization polkitqt1-temporaryauthorization.h TemporaryAuthorization
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * \brief This class represents PolicyKit temporary authorization
 *
 * This class encapsulates the PolkitTemporaryAuthorization interface.
 */
class POLKITQT1_CORE_EXPORT TemporaryAuthorization
{
public:
    typedef QList< TemporaryAuthorization > List;
    TemporaryAuthorization();
    /**
     * Creates TemporaryAuthorization object from PolkitTemporaryAuthorization
     *
     * \warning It shouldn't be used directly unless you are completely aware of what are you doing
     *
     * \param pkTemporaryAuthorization PolkitTemporaryAuthorization object
     * \param parent
     */
    explicit TemporaryAuthorization(PolkitTemporaryAuthorization *pkTemporaryAuthorization);
    TemporaryAuthorization(const TemporaryAuthorization &other);

    ~TemporaryAuthorization();

    TemporaryAuthorization &operator=(const TemporaryAuthorization &other);

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
    Subject subject() const;

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
    class Data;
    QSharedDataPointer< Data > d;
};
}

Q_DECLARE_METATYPE(PolkitQt1::TemporaryAuthorization::List)

#endif // TEMPORARYAUTHORIZATION_H
