/*
    This file is part of the PolKit1-qt project
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polkitqt1-temporaryauthorization.h"
#include "polkitqt1-authority.h"

#include <polkit/polkit.h>

namespace PolkitQt1
{

class Q_DECL_HIDDEN TemporaryAuthorization::Data : public QSharedData
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
    d->id = QString::fromUtf8(polkit_temporary_authorization_get_id(pkTemporaryAuthorization));
    d->actionId = QString::fromUtf8(polkit_temporary_authorization_get_action_id(pkTemporaryAuthorization));
    d->subject = Subject::fromString(polkit_subject_to_string(polkit_temporary_authorization_get_subject(pkTemporaryAuthorization)));
    d->timeObtained = QDateTime::fromSecsSinceEpoch(polkit_temporary_authorization_get_time_obtained(pkTemporaryAuthorization));
    d->timeExpires = QDateTime::fromSecsSinceEpoch(polkit_temporary_authorization_get_time_expires(pkTemporaryAuthorization));
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
