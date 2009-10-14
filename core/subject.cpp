/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
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

#include "subject.h"

#include <polkit/polkit.h>

using namespace PolkitQt;

Subject::Subject()
        : m_subject(0)
{
    g_type_init();
}

Subject::Subject(PolkitSubject *subject)
        : m_subject(subject)
{
    g_type_init();
}

Subject::~Subject()
{
    g_object_unref(m_subject);
}

PolkitSubject * Subject::subject()
{
    return m_subject;
}

QString Subject::toString() const
{
    Q_ASSERT(m_subject);
    return QString::fromUtf8(polkit_subject_to_string(m_subject));
}

Subject * Subject::fromString(const QString & string)
{
   // should be in polkit library!!! but for strange reason it's neccessary to have it here
    g_type_init();

    Subject *subject = new Subject();
    GError *error = NULL;
    subject->m_subject = polkit_subject_from_string(string.toUtf8().data(), &error);
    if (error != NULL)
    {
        return NULL;
    }
    return subject;
}

UnixProcess::UnixProcess(qint64 pid)
        : Subject()
{
    m_subject = polkit_unix_process_new(pid);
}

UnixProcess::UnixProcess(qint64 pid, quint64 startTime)
        : Subject()
{
    m_subject = polkit_unix_process_new_full(pid, startTime);
}

UnixProcess::UnixProcess(PolkitUnixProcess *pkUnixProcess)
        : Subject((PolkitSubject *)pkUnixProcess)
{

}

qint64 UnixProcess::pid() const
{
    return polkit_unix_process_get_pid((PolkitUnixProcess *) m_subject);
}

qint64 UnixProcess::startTime() const
{
    return polkit_unix_process_get_start_time((PolkitUnixProcess *) m_subject);
}

void UnixProcess::setPid(qint64 pid)
{
    polkit_unix_process_set_pid((PolkitUnixProcess *) m_subject, pid);
}

// ----- SystemBusName
SystemBusName::SystemBusName(const QString &name)
        : Subject()
{
    m_subject = polkit_system_bus_name_new(name.toUtf8().data());
}

SystemBusName::SystemBusName(PolkitSystemBusName *pkSystemBusName)
        : Subject((PolkitSubject *) pkSystemBusName)
{

}

QString SystemBusName::name() const
{
    return QString::fromUtf8(polkit_system_bus_name_get_name((PolkitSystemBusName *) m_subject));
}

void SystemBusName::setName(const QString &name)
{
    polkit_system_bus_name_set_name((PolkitSystemBusName *) m_subject, name.toUtf8().data());
}

// ----- SystemSession
UnixSession::UnixSession(const QString &sessionId)
        : Subject()
{
    m_subject = polkit_unix_session_new(sessionId.toUtf8().data());
}

UnixSession::UnixSession(qint64 pid)
        : Subject()
{
    // TODO: error handling
    m_subject = polkit_unix_session_new_for_process_sync(pid, NULL, NULL);
}

UnixSession::UnixSession(PolkitSystemBusName *pkUnixSession)
        : Subject((PolkitSubject *) pkUnixSession)
{

}

QString UnixSession::sessionId() const
{
    return QString::fromUtf8(polkit_unix_session_get_session_id((PolkitUnixSession *) m_subject));
}

void UnixSession::setSessionId(const QString &sessionId)
{
    polkit_unix_session_set_session_id((PolkitUnixSession *) m_subject, sessionId.toUtf8().data());
}
