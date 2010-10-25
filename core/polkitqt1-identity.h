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

#ifndef POLKITQT1_IDENTITY_H
#define POLKITQT1_IDENTITY_H

#include "polkitqt1-export.h"

#include <unistd.h>

#include <QtCore/QObject>
#include <QtCore/QSharedData>

typedef struct _PolkitIdentity PolkitIdentity;
typedef struct _PolkitUnixUser PolkitUnixUser;
typedef struct _PolkitUnixGroup PolkitUnixGroup;

/**
 * \namespace PolkitQt1 PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt1
{

class UnixUserIdentity;
class UnixGroupIdentity;

/**
 * \class Identity polkitqt1-identity.h Identity
 * \author Lukas Tinkl <ltinkl@redhat.com>
 *
 * This class encapsulates the PolkitIdentity interface.
 *
 * \brief Abstract class representing identities
 *
 * \see UnixGroup
 * \see UnixUser
 */
class POLKITQT1_EXPORT Identity
{
public:
    typedef QList< Identity > List;

    Identity();
    explicit Identity(PolkitIdentity *polkitIdentity);
    Identity(const Identity &other);

    ~Identity();

    Identity &operator=(const Identity &other);

    bool isValid() const;

    /**
     * Serialization of object to the string
     *
     * \return Serialized Identity object
     */
    QString toString() const;

    /**
     * Creates the Identity object from string representation
     *
     * \param string string representation of the object
     *
     * \return Pointer to new Identity instance
     */
    static Identity fromString(const QString &string);

    UnixUserIdentity toUnixUserIdentity();
    UnixGroupIdentity toUnixGroupIdentity();

    /**
     * Gets PolkitIdentity object.
     *
     * \warning It shouldn't be used directly unless you are completely aware of what are you doing
     *
     * \return Pointer to PolkitIdentity instance
     */
    PolkitIdentity *identity() const;
protected:
    void setIdentity(PolkitIdentity *identity);

private:
    class Data;
    QExplicitlySharedDataPointer< Data > d;
};

/**
  * \class UnixUserIdentity polkitqt1-identity.h Identity
  *
  * An object representing a user identity on a UNIX system.
  *
  * \brief UNIX user identity
  * \sa Identity
  */
class POLKITQT1_EXPORT UnixUserIdentity : public Identity
{
public:
    UnixUserIdentity();
    /**
     * Creates UnixUser object by UID of the user
     *
     * \param uid user id
     */
    explicit UnixUserIdentity(uid_t uid);

    /**
     * Creates UnixUser object by unix name of the user
     *
     * \param name Unix name
     */
    explicit UnixUserIdentity(const QString &name);

    /**
     * Creates UnixUser object from PolkitUnixUser object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixUser The PolkitUnixUser object
     */
    explicit UnixUserIdentity(PolkitUnixUser *pkUnixUser);

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
  * \class UnixGroupIdentity polkitqt1-identity.h Identity
  *
  * An object representing a group identity on a UNIX system.
  *
  * \brief UNIX group identity
  * \sa Identity
  */
class POLKITQT1_EXPORT UnixGroupIdentity : public Identity
{
public:
    UnixGroupIdentity();
    /**
     * Creates UnixGroup object by GID of the group
     *
     * \param gid group id
     */
    explicit UnixGroupIdentity(gid_t gid);

    /**
     * Creates UnixGroup object by unix name of the group
     *
     * \param name group name
     */
    explicit UnixGroupIdentity(const QString &name);

    /**
     * Creates UnixGroup object from PolkitUnixGroup object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixGroup The PolkitUnixGroup object
     */
    explicit UnixGroupIdentity(PolkitUnixGroup *pkUnixGroup);

    /**
     * Gets a group id
     *
     * \return group id
     */
    gid_t gid() const;

    /**
     * Sets the id of group
     *
     * \param gid group id
     */
    void setGid(gid_t gid);
};

}

#endif // POLKIT_QT_IDENTITY_H
