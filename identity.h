/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Lukas Tinkl <ltinkl@redhat.com>
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

#ifndef POLKIT_QT_IDENTITY_H
#define POLKIT_QT_IDENTITY_H

#include "export.h"

#include <unistd.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <polkit/polkit.h>

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
 * \class Identity identity.h Identity
 * \author Lukas Tinkl <ltinkl@redhat.com>
 *
 * Identity is an abstract class for representing identities
 *
 * This class encapsulates the PolkitIdentity interface.
 *
 * \see UnixGroup
 * \see UnixUser
 */
class POLKIT_QT_EXPORT Identity: public QObject
{
    Q_OBJECT
public:
    /**
     * Creates Identity object from PolkitIdentity
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param polkitIdentity PolkitIdentity object
     */
    Identity( PolkitIdentity * polkitIdentity, QObject *parent = 0 );
    ~Identity();

    /**
     * Serialization of object to the string
     *
     * \return Serialized Identity object
     */
    QString toString() const;

    /**
     * Creates the Identity object from string reprezentation
     *
     * \param String reprezentation of the object
     *
     * \return Pointer to new Identity instance
     */
    static Identity * fromString(const QString & string);

    /**
     * Gets PolkitIdentity object.
     *
     * \warning It shouldn't be used directly unless you are completely aware of what are you doing
     *
     * \return Pointer to PolkitIdentity instance
     */
    PolkitIdentity * identity();
protected:
    PolkitIdentity * m_identity;
    explicit Identity(QObject *parent = 0);
};

/**
  * An object representing a user identity on a UNIX system.
  */
class POLKIT_QT_EXPORT UnixUser: public Identity
{
    Q_OBJECT
public:
    /**
     * Creates UnixUser object by UID of the user
     *
     * \param uid user id
     */
    UnixUser(uid_t uid, QObject *parent = 0);

    /**
     * Creates UnixUser object by unix name of the user
     *
     * \param name Unix name
     */
    UnixUser(const QString & name, QObject *parent = 0);

    /**
     * Creates UnixUser object from PolkitUnixUser object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixUser The PolkitUnixUser object
     */
    UnixUser(PolkitUnixUser *pkUnixUser, QObject *parent = 0);

    /**
     * Gets an user id
     *
     * \return user id
     */
    uid_t uid() const;

    /**
     * Sets the id of user
     *
     * \param uid user id
     */
    void setUid(uid_t uid);
};

/**
  * An object representing a group identity on a UNIX system.
  */
class POLKIT_QT_EXPORT UnixGroup: public Identity
{
    Q_OBJECT
public:
    /**
     * Creates UnixGroup object by GID of the group
     *
     * \param uid group id
     */
    UnixGroup(gid_t uid, QObject *parent = 0);

    /**
     * Creates UnixGroup object by unix name of the group
     *
     * \param name group name
     */
    UnixGroup(const QString & name, QObject *parent = 0);

    /**
     * Creates UnixGroup object from PolkitUnixGroup object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixGroup The PolkitUnixGroup object
     */
    UnixGroup(PolkitUnixGroup *pkUnixGroup, QObject *parent = 0);

    /**
     * Gets a group id
     *
     * \return group id
     */
    gid_t gid() const;

    /**
     * Sets the id of group
     *
     * \param uid group id
     */
    void setGid(gid_t gid);
};

}

#endif // POLKIT_QT_IDENTITY_H
