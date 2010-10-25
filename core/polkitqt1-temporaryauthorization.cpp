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

#include "polkitqt1-temporaryauthorization.h"
#include "polkitqt1-authority.h"

#include <polkit/polkit.h>

namespace PolkitQt1
{

class TemporaryAuthorization::Data : public QSharedData
{
public:
    Data() {}
    Data(const Data& other)
        : QSharedData(other)
        , id(other.id)
        , actionId(other.actionId)
        , subject(other.subject)
        , timeObtained(other.timeObtained)
        , timeExpires(other.timeExpires)
    {
    }
    ~Data() {}

    QString id;
    QString actionId;
    Subject subject;
    QDateTime timeObtained;
    QDateTime timeExpires;
};

TemporaryAuthorization::TemporaryAuthorization(PolkitTemporaryAuthorization *pkTemporaryAuthorization)
        : d(new Data)
{
    g_type_init();
    d->id = QString::fromUtf8(polkit_temporary_authorization_get_id(pkTemporaryAuthorization));
    d->actionId = QString::fromUtf8(polkit_temporary_authorization_get_action_id(pkTemporaryAuthorization));
    d->subject = Subject::fromString(polkit_subject_to_string(polkit_temporary_authorization_get_subject(pkTemporaryAuthorization)));
    d->timeObtained = QDateTime::fromTime_t(polkit_temporary_authorization_get_time_obtained(pkTemporaryAuthorization));
    d->timeExpires = QDateTime::fromTime_t(polkit_temporary_authorization_get_time_expires(pkTemporaryAuthorization));
    g_object_unref(pkTemporaryAuthorization);
}

TemporaryAuthorization::TemporaryAuthorization(const PolkitQt1::TemporaryAuthorization& other)
        : d(other.d)
{

}

TemporaryAuthorization::TemporaryAuthorization()
        : d(new Data)
{

}

TemporaryAuthorization& TemporaryAuthorization::operator=(const PolkitQt1::TemporaryAuthorization& other)
{
    d = other.d;
    return *this;
}

TemporaryAuthorization::~TemporaryAuthorization()
{
}

QString TemporaryAuthorization::id() const
{
    return d->id;
}

QString TemporaryAuthorization::actionId() const
{
    return d->actionId;
}

Subject TemporaryAuthorization::subject() const
{
    //qFatal(polkit_subject_to_string(polkit_temporary_authorization_get_subject(d->temporaryAuthorization)));
    return d->subject;//Subject::fromString(polkit_subject_to_string(d->subject));
}

QDateTime TemporaryAuthorization::obtainedAt() const
{
    return d->timeObtained;
}

QDateTime TemporaryAuthorization::expirationTime() const
{
    return d->timeExpires;
}

bool TemporaryAuthorization::revoke()
{
    Authority::instance()->revokeTemporaryAuthorization(id());
    return true;
}

}
