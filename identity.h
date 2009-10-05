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
 * This class encapsulates the PolkitIdentity interface.
 *
 * \see UnixGroup
 * \see UnixUser
 */
class POLKIT_QT_EXPORT Identity: public QObject
{
    Q_OBJECT
public:
    Identity( PolkitIdentity * polkitIdentity, QObject *parent = 0 );
    ~Identity();
    QString toString() const;
    static Identity * fromString(const QString & string);
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
    UnixUser(uid_t uid, QObject *parent = 0);
    UnixUser(const QString & name, QObject *parent = 0);
    uid_t uid() const;
    void setUid(uid_t uid);
};

/**
  * An object representing a group identity on a UNIX system.
  */
class POLKIT_QT_EXPORT UnixGroup: public Identity
{
    Q_OBJECT
public:
    UnixGroup(gid_t uid, QObject *parent = 0);
    UnixGroup(const QString & name, QObject *parent = 0);
    gid_t gid() const;
    void setGid(gid_t gid);
};

}

#endif // POLKIT_QT_IDENTITY_H
