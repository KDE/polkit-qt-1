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

#include "temporaryauthorization.h"
#include "authority.h"

#include <polkit/polkit.h>

using namespace PolkitQt;

TemporaryAuthorization::TemporaryAuthorization(PolkitTemporaryAuthorization *pkTemporaryAuthorization, QObject *parent) : QObject(parent)
{
    g_type_init();
    m_id = QString::fromUtf8(polkit_temporary_authorization_get_id(pkTemporaryAuthorization));
    m_actionId = QString::fromUtf8(polkit_temporary_authorization_get_action_id(pkTemporaryAuthorization));
    m_subject = Subject::fromString(polkit_subject_to_string(polkit_temporary_authorization_get_subject(pkTemporaryAuthorization)));
    m_timeObtained = QDateTime::fromTime_t(polkit_temporary_authorization_get_time_obtained(pkTemporaryAuthorization));
    m_timeExpires = QDateTime::fromTime_t(polkit_temporary_authorization_get_time_expires(pkTemporaryAuthorization));
    g_object_unref(pkTemporaryAuthorization);
}

TemporaryAuthorization::~TemporaryAuthorization()
{
    delete m_subject;
}

QString TemporaryAuthorization::id() const
{
    return m_id;
}

QString TemporaryAuthorization::actionId() const
{
    return m_actionId;
}

Subject *TemporaryAuthorization::subject()
{
    //qFatal(polkit_subject_to_string(polkit_temporary_authorization_get_subject(m_temporaryAuthorization)));
    return m_subject;//Subject::fromString(polkit_subject_to_string(m_subject));
}

QDateTime TemporaryAuthorization::timeObtained() const
{
    return m_timeObtained;
}

QDateTime TemporaryAuthorization::timeExpires() const
{
    return m_timeExpires;
}

bool TemporaryAuthorization::revoke()
{
    Authority::instance()->revokeTemporaryAuthorization(id());
}
