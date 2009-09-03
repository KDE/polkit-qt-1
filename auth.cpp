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

Auth::Result Auth::isCallerAuthorized(const QString &actionId, qint64 pid, AuthorizationFlags flags)
{
    PolkitSubject *subject;
    Result result;

    // should be in polkit library!!! but for strange reason it's neccesary to have it here
    g_type_init();

    subject = polkit_unix_process_new(pid);

    result = Auth::isCallerAuthorized(actionId, subject, flags);
    
    g_object_unref(subject);
    
    return result;
}

Auth::Result Auth::isCallerAuthorized(const QString &actionId, const QString &dbusName, AuthorizationFlags flags)
{
    PolkitSubject *subject;
    Result result;

    // should be in polkit library!!! but for strange reason it's neccesary to have it here
    g_type_init();

    subject = polkit_system_bus_name_new(dbusName.toAscii().data());

    result = Auth::isCallerAuthorized(actionId, subject, flags);
    
    g_object_unref(subject);
    
    return result;
}

Auth::Result Auth::isCallerAuthorized(const QString &actionId, PolkitSubject *subject, AuthorizationFlags flags)
{
    PolkitAuthorizationResult *pk_result;
    GError *error = NULL;
    
    if (Authority::instance()->hasError()) {
        return Auth::Unknown;
    }

    if (!subject) {
        qWarning("No subject given for this target.");
        return Auth::Unknown;
    }

    qDebug("Subject: %s, actionId: %s", polkit_subject_to_string(subject), actionId.toAscii().data());

    // FIXME: flags handling
    pk_result = polkit_authority_check_authorization_sync(Authority::instance()->getPolkitAuthority(),
                subject, 
                actionId.toAscii().data(), 
                NULL,
                (PolkitCheckAuthorizationFlags)(int)flags,
                NULL,
                &error);

    if (error != NULL) {
        qWarning("Authority checking failed with message: %s", error->message);
        g_error_free(error);
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

bool Auth::registerAuthenticationAgent(PolkitSubject *subject, const QString &locale, const QString &objectPath) 
{
    gboolean result;
    GError *error = NULL;
    
    if (Authority::instance()->hasError()) {
        return false;
    }

    if (!subject) {
        qWarning("No subject given for this target.");
        return false;
    }

    qDebug("Subject: %s, objectPath: %s", polkit_subject_to_string(subject), objectPath.toAscii().data());
    
    result = polkit_authority_register_authentication_agent_sync(Authority::instance()->getPolkitAuthority(),
						   subject, locale.toAscii().data(),
						   objectPath.toAscii().data(), NULL, &error);
						   
    if (error) {
        qWarning("Authentication agent registration failed with message: %s", error->message);
        g_error_free (error);
        return false;
    }
    
    return result;
}
