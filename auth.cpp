/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
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

#include "auth.h"
#include "action.h"
#include "authority.h"

#include <limits.h>
#include <polkit/polkit.h>
#include <glib-object.h>

#include <QtCore/QDebug>

using namespace PolkitQt;

namespace PolkitQt {
namespace Auth {

    Result polkitResultToResult(PolkitAuthorizationResult *result);
}
}

bool Auth::computeAuth(const QString &actionId, qint64 pid)
{
    if (isCallerAuthorized(actionId, pid) == Yes)
        return true;
    else
        return false;
}

Auth::Result Auth::isCallerAuthorized(const QString &actionId, qint64 pid)
{
    PolkitSubject *subject;

    qDebug("Auth::isCallerAuthorized(%s, %lld)", actionId.toAscii().data(), pid);

    subject = polkit_unix_process_new(pid);

    return Auth::isCallerAuthorized(actionId, subject);
}

Auth::Result Auth::isCallerAuthorized(const QString &actionId, const QString &dbusName)
{
    PolkitSubject *subject;

    qDebug("Auth::isCallerAuthorized(%s, %s)", actionId.toAscii().data(), dbusName.toAscii().data());

    // wtf? should be in polkit library!!!
    g_type_init();

    subject = polkit_system_bus_name_new(dbusName.toAscii().data());

    return Auth::isCallerAuthorized(actionId, subject);
}

Auth::Result Auth::isCallerAuthorized(const QString &actionId, PolkitSubject *subject)
{
    PolkitAuthorizationResult *pk_result;
    GError *error = NULL;

    if (Authority::instance()->hasError()) {
        return Auth::Unknown;
    }

    if (!subject) {
        qWarning("Cannot get PolkitSubject for this target");
        return Auth::Unknown;
    }

    qDebug("Subject: %s (%s), actionId: %s", polkit_subject_to_string(subject), subject, actionId.toAscii().data());

    pk_result = polkit_authority_check_authorization_sync(Authority::instance()->getPolkitAuthority(),
                subject, 
                actionId.toAscii().data(), 
                NULL,
                POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION,
                NULL,
                &error);

    if (error) {
        qWarning("Authority checking failed with message: %s", error->message);
        g_error_free (error);
        return Auth::Unknown;
    }

    if (!pk_result)
        return Auth::Unknown;
    else
        return Auth::polkitResultToResult(pk_result);
}

Auth::Result Auth::polkitResultToResult(PolkitAuthorizationResult *result) {

    if (polkit_authorization_result_get_is_challenge(result))
        return Auth::Challenge;
    else if (polkit_authorization_result_get_is_authorized(result))
        return Auth::Yes;
    else
        return Auth::No;
}
