/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polkitqt1-identity.h"

#include <polkit/polkit.h>

#include <QDebug>

namespace PolkitQt1
{

class Q_DECL_HIDDEN Identity::Data : public QSharedData
{
public:
    Data() : identity(nullptr) {}
    Data(const Data& other)
        : QSharedData(other)
        , identity(other.identity)
    {
        if (identity) {
            g_object_ref(identity);
        }
    }
    ~Data()
    {
        if (identity) {
            g_object_unref(identity);
        }
    }

    PolkitIdentity *identity;
};

Identity::Identity()
        : d(new Data)
{
}

Identity::Identity(PolkitIdentity *polkitIdentity)
        : d(new Data)
{
    d->identity = polkitIdentity;

    if (d->identity) {
        g_object_ref(d->identity);
    }
}

Identity::Identity(const PolkitQt1::Identity& other)
        : d(other.d)
{

}

Identity::~Identity()
{
}

Identity& Identity::operator=(const PolkitQt1::Identity& other)
{
    d = other.d;
    return *this;
}

bool Identity::isValid() const
{
    return d->identity != nullptr;
}

PolkitIdentity *Identity::identity() const
{
    return d->identity;
}

void Identity::setIdentity(PolkitIdentity *identity)
{
    if (d->identity == identity) {
        return;
    }

    if (d->identity) {
        g_object_unref(d->identity);
    }

    d->identity = identity;

    if (d->identity) {
        g_object_ref(d->identity);
    }
}

QString Identity::toString() const
{
    Q_ASSERT(d->identity);
    return QString::fromUtf8(polkit_identity_to_string(d->identity));
}

Identity Identity::fromString(const QString &string)
{
    GError *error = nullptr;
    PolkitIdentity *pkIdentity = polkit_identity_from_string(string.toUtf8().data(), &error);
    if (error != nullptr) {
        qWarning() << QString("Cannot create Identity from string: %1").arg(error->message);
        return Identity();
    }
    return Identity(pkIdentity);
}

UnixGroupIdentity Identity::toUnixGroupIdentity()
{
    UnixGroupIdentity *ugid = static_cast< UnixGroupIdentity* >(this);
    if (!ugid) {
        return UnixGroupIdentity();
    }

    return *ugid;
}

UnixUserIdentity Identity::toUnixUserIdentity()
{
    UnixUserIdentity *uuid = static_cast< UnixUserIdentity* >(this);
    if (!uuid) {
        return UnixUserIdentity();
    }

    return *uuid;
}

UnixUserIdentity::UnixUserIdentity(const QString &name)
        : Identity()
{
    GError *error = nullptr;
    setIdentity(polkit_unix_user_new_for_name(name.toUtf8().data(), &error));
    if (error != nullptr) {
        qWarning() << QString("Cannot create UnixUserIdentity: %1").arg(error->message);
        setIdentity(nullptr);
    }
}

UnixUserIdentity::UnixUserIdentity(uid_t uid)
        : Identity()
{
    setIdentity(polkit_unix_user_new(uid));
}

UnixUserIdentity::UnixUserIdentity(PolkitUnixUser *pkUnixUser)
        : Identity((PolkitIdentity *)pkUnixUser)
{

}

UnixUserIdentity::UnixUserIdentity()
        : Identity()
{

}

uid_t UnixUserIdentity::uid() const
{
    return polkit_unix_user_get_uid((PolkitUnixUser *) identity());
}

void UnixUserIdentity::setUid(uid_t uid)
{
    polkit_unix_user_set_uid((PolkitUnixUser *) identity(), uid);
}

UnixGroupIdentity::UnixGroupIdentity(const QString &name)
        : Identity()
{
    GError *error = nullptr;
    setIdentity(polkit_unix_group_new_for_name(name.toUtf8().data(), &error));
    if (error != nullptr) {
        qWarning() << QString("Cannot create UnixGroupIdentity: %1").arg(error->message);
        setIdentity(nullptr);
    }
}

UnixGroupIdentity::UnixGroupIdentity(gid_t gid)
        : Identity()
{
    setIdentity(polkit_unix_group_new(gid));
}

UnixGroupIdentity::UnixGroupIdentity(PolkitUnixGroup *pkUnixGroup)
        : Identity((PolkitIdentity *) pkUnixGroup)
{

}

UnixGroupIdentity::UnixGroupIdentity()
        : Identity()
{

}

gid_t UnixGroupIdentity::gid() const
{
    return polkit_unix_group_get_gid((PolkitUnixGroup *) identity());
}

void UnixGroupIdentity::setGid(gid_t gid)
{
    polkit_unix_group_set_gid((PolkitUnixGroup *) identity(), gid);
}

}
