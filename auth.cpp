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

Auth::Result Auth::checkAuthorization(const QString &actionId, Subject *subject, AuthorizationFlags flags)
{
    PolkitAuthorizationResult *pk_result;
    GError *error = NULL;
    
    if (Authority::instance()->hasError()) {
        return Auth::Unknown;
    }
    
    // TODO: subject check

    pk_result = polkit_authority_check_authorization_sync(Authority::instance()->getPolkitAuthority(),
                subject->subject(), 
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

QStringList Auth::enumerateActions()
{
    if (Authority::instance()->hasError())
        return QStringList();

    GError *error = NULL;

    GList * glist = polkit_authority_enumerate_actions_sync(Authority::instance()->getPolkitAuthority(),
                                                            NULL,
                                                            &error);

    if (error != NULL)
    {
        qWarning("Enumerating actions failed with message: %s", error->message);
        g_error_free(error);
        return QStringList();
    }

    //FIXME: g_object_unref!
    QStringList result;
    GList * glist2 = glist;
    for (gpointer i = glist->data; glist; glist = g_list_next(glist))
    {
        result.append(QString::fromUtf8(polkit_action_description_get_action_id((PolkitActionDescription*)i)));
        g_object_unref(i);
    }

    g_list_free(glist2);

    return result;
}

bool Auth::registerAuthenticationAgent(Subject *subject, const QString &locale, const QString &objectPath) 
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

    qDebug("Subject: %s, objectPath: %s", subject->toString().toAscii().data(), objectPath.toAscii().data());
    
    result = polkit_authority_register_authentication_agent_sync(Authority::instance()->getPolkitAuthority(),
						   subject->subject(), locale.toAscii().data(),
						   objectPath.toAscii().data(), NULL, &error);
						   
    if (error) {
        qWarning("Authentication agent registration failed with message: %s", error->message);
        g_error_free (error);
        return false;
    }
    
    return result;
}

bool Auth::unregisterAuthenticationAgent(Subject *subject, const QString &objectPath)
{
    if (Authority::instance()->hasError())
        return false;

    if (!subject)
    {
        qWarning("No subject given for this target.");
        return false;
    }

    GError *error = NULL;

    qDebug("Unregistering agent, subject: %s", subject->toString().toAscii().data());

    bool result = polkit_authority_unregister_authentication_agent_sync(Authority::instance()->getPolkitAuthority(),
                                                                        subject->subject(),
                                                                        objectPath.toUtf8().data(),
                                                                        NULL,
                                                                        &error);

    if (error != NULL)
    {
        qWarning("Unregistering agent failed with message: %s", error->message);
        g_error_free(error);
        return false;
    }

    return result;
}

bool Auth::authenticationAgentResponse(const QString & cookie, Identity * identity)
{
    if (Authority::instance()->hasError())
        return false;

    if (cookie.isEmpty() || !identity)
    {
        qWarning("Cookie or identity is empty!");
        return false;
    }

    GError *error = NULL;

    qDebug() << "Auth agent response, cookie: " << cookie << ", identity:" << identity->toString();

    bool result = polkit_authority_authentication_agent_response_sync(Authority::instance()->getPolkitAuthority(),
                                                                      cookie.toUtf8().data(),
                                                                      identity->identity(),
                                                                      NULL,
                                                                      &error);
    if (error != NULL)
    {
        qWarning("Auth agent response failed with: %s", error->message);
        g_error_free(error);
        return false;
    }

    return result;
}
