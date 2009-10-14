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

#include "identity.h"

#include <polkit/polkit.h>
#include <glib-object.h>

using namespace PolkitQt;

class Identity::Private
{
    public:
        Private(PolkitIdentity *i) : identity(i) {}

        PolkitIdentity *identity;
};

Identity::Identity()
        : d(new Private(0))
{
    g_type_init();
}

Identity::Identity(PolkitIdentity * polkitIdentity)
        : d(new Private(polkitIdentity))
{
    g_type_init();
}

Identity::~Identity()
{
    g_object_unref(d->identity);
}

PolkitIdentity * Identity::identity() const
{
    return d->identity;
}

void Identity::setIdentity(PolkitIdentity* identity)
{
    d->identity = identity;
}

QString Identity::toString() const
{
    Q_ASSERT(d->identity);
    return QString::fromUtf8(polkit_identity_to_string(d->identity));
}

Identity * Identity::fromString(const QString & string)
{
    Identity *identity = new Identity();
    identity->d->identity = polkit_identity_from_string(string.toUtf8().data(), NULL);
    return identity;
}

UnixUser::UnixUser(const QString & name)
        : Identity()
{
    GError *error = NULL;
    setIdentity(polkit_unix_user_new_for_name(name.toUtf8().data(), &error));
}

UnixUser::UnixUser(uid_t uid)
        : Identity()
{
    setIdentity(polkit_unix_user_new(uid));
}

UnixUser::UnixUser(PolkitUnixUser *pkUnixUser)
        : Identity((PolkitIdentity *)pkUnixUser)
{

}

uid_t UnixUser::uid() const
{
    return polkit_unix_user_get_uid((PolkitUnixUser *) identity());
}

void UnixUser::setUid(uid_t uid)
{
    polkit_unix_user_set_uid((PolkitUnixUser *) identity(), uid);
}

UnixGroup::UnixGroup(const QString & name)
        : Identity()
{
    GError *error = NULL;
    setIdentity(polkit_unix_group_new_for_name(name.toUtf8().data(), &error));
}

UnixGroup::UnixGroup(gid_t gid)
        : Identity()
{
    setIdentity(polkit_unix_group_new(gid));
}

UnixGroup::UnixGroup(PolkitUnixGroup *pkUnixGroup)
        : Identity((PolkitIdentity *) pkUnixGroup)
{

}

gid_t UnixGroup::gid() const
{
    return polkit_unix_group_get_gid((PolkitUnixGroup *) identity());
}

void UnixGroup::setGid(gid_t gid)
{
    polkit_unix_group_set_gid((PolkitUnixGroup *) identity(), gid);
}
