/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polkitqt1-subject.h"
#include "polkitqt1-identity.h"
#include "polkitqt1-config.h"

#include <QDebug>
#include <polkit/polkit.h>

namespace PolkitQt1
{

class Q_DECL_HIDDEN Subject::Data : public QSharedData
{
public:
    Data()
        : QSharedData()
        , subject(nullptr)
    {}
    Data(const Data& other)
        : QSharedData(other)
        , subject(other.subject)
    {
        g_object_ref(subject);
    }
    ~Data()
    {
        g_object_unref(subject);
    }

    PolkitSubject *subject;
};

Subject::Subject()
        : d(new Data)
{
}

Subject::Subject(PolkitSubject *subject)
        : d(new Data)
{
    d->subject = subject;
    
    if (d->subject != nullptr) {
        g_object_ref(d->subject);
    }
}

Subject::Subject(const PolkitQt1::Subject& other)
        : d(other.d)
{

}

Subject& Subject::operator=(const PolkitQt1::Subject& other)
{
    d = other.d;
    return *this;
}

Subject::~Subject()
{
}

bool Subject::isValid() const
{
    return d->subject != nullptr;
}

PolkitSubject *Subject::subject() const
{
    return d->subject;
}

void Subject::setSubject(PolkitSubject *subject)
{
    if (d->subject != nullptr) {
        g_object_unref(d->subject);
    }
    d->subject = subject;
}

QString Subject::toString() const
{
    Q_ASSERT(d->subject);
    return QString::fromUtf8(polkit_subject_to_string(d->subject));
}

Subject Subject::fromString(const QString &string)
{
    Subject subject;
    GError *error = nullptr;
    subject.d->subject = polkit_subject_from_string(string.toUtf8().data(), &error);
    if (error != nullptr) {
        qWarning() << QString("Cannot create Subject from string: %1").arg(error->message);
        return nullptr;
    }
    return subject;
}

UnixProcessSubject::UnixProcessSubject(qint64 pid)
        : Subject()
{
    setSubject(polkit_unix_process_new_for_owner(pid, 0, -1));
}

UnixProcessSubject::UnixProcessSubject(qint64 pid, quint64 startTime)
        : Subject()
{
    setSubject(polkit_unix_process_new_for_owner(pid, startTime, -1));
}

UnixProcessSubject::UnixProcessSubject(PolkitUnixProcess *pkUnixProcess)
        : Subject((PolkitSubject *) pkUnixProcess)
{

}

qint64 UnixProcessSubject::pid() const
{
    return polkit_unix_process_get_pid((PolkitUnixProcess *) subject());
}

qint64 UnixProcessSubject::startTime() const
{
    return polkit_unix_process_get_start_time((PolkitUnixProcess *) subject());
}

qint64 UnixProcessSubject::uid() const
{
    return polkit_unix_process_get_uid((PolkitUnixProcess *) subject());
}

void UnixProcessSubject::setPid(qint64 pid)
{
    polkit_unix_process_set_pid((PolkitUnixProcess *) subject(), pid);
}

// ----- SystemBusName
SystemBusNameSubject::SystemBusNameSubject(const QString &name)
        : Subject()
{
    setSubject(polkit_system_bus_name_new(name.toUtf8().data()));
}

SystemBusNameSubject::SystemBusNameSubject(PolkitSystemBusName *pkSystemBusName)
        : Subject((PolkitSubject *) pkSystemBusName)
{

}

QString SystemBusNameSubject::name() const
{
    return QString::fromUtf8(polkit_system_bus_name_get_name((PolkitSystemBusName *) subject()));
}

void SystemBusNameSubject::setName(const QString &name)
{
    polkit_system_bus_name_set_name((PolkitSystemBusName *) subject(), name.toUtf8().data());
}

UnixUserIdentity SystemBusNameSubject::user()
{
#if HAVE_POLKIT_SYSTEM_BUS_NAME_GET_USER_SYNC
    return UnixUserIdentity(polkit_system_bus_name_get_user_sync((PolkitSystemBusName *) subject(), nullptr, nullptr));
#else
    qWarning("Polkit is too old, returning invalid user from SystemBusNameSubject::user()!");
    return UnixUserIdentity();
#endif
}

// ----- SystemSession
UnixSessionSubject::UnixSessionSubject(const QString &sessionId)
        : Subject()
{
    setSubject(polkit_unix_session_new(sessionId.toUtf8().data()));
}

UnixSessionSubject::UnixSessionSubject(qint64 pid)
        : Subject()
{
    GError *error = nullptr;
    setSubject(polkit_unix_session_new_for_process_sync(pid, nullptr, &error));
    if (error != nullptr) {
        qWarning() << QString("Cannot create unix session: %1").arg(error->message);
        setSubject(nullptr);
    }
}

UnixSessionSubject::UnixSessionSubject(PolkitSystemBusName *pkUnixSession)
        : Subject((PolkitSubject *) pkUnixSession)
{

}

QString UnixSessionSubject::sessionId() const
{
    return QString::fromUtf8(polkit_unix_session_get_session_id((PolkitUnixSession *) subject()));
}

void UnixSessionSubject::setSessionId(const QString &sessionId)
{
    polkit_unix_session_set_session_id((PolkitUnixSession *) subject(), sessionId.toUtf8().data());
}

}
