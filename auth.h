/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
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

#ifndef POLKIT_QT_AUTH_H
#define POLKIT_QT_AUTH_H

#include "export.h"

#include <QtCore/QString>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <polkit/polkit.h>

namespace PolkitQt
{

/**
 * \namespace
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief Functions used to obtain authorizations
 *
 * This namespace contains class is a simple wrapper around the DBus interface
 * org.freedesktop.PolicyKit.AuthenticationAgent to make usage of
 * policykit easier in Qt/KDE worlds
 */
namespace Auth
{

/**
 * Defines the result given by an action
 */
enum Result {
    /** Result unknown */
    Unknown = 0x00,
    /** The subject is authorized for the specified action  */
    Yes = 0x01,
    /** The subject is authorized for the specified action  */
    No = 0x02,
    /** The subject is authorized if more information is provided */
    Challenge = 0x03
};

/**
 * TODO: description
 * \see Action
 *
 * \param actionId id of the action in question (i.e. org.freedesktop.policykit.read)
 * \param pid Process id of the application in question
 * \return \c true if the user is authorized
 *         \c false if the user is not authorized
 */
POLKIT_QT_EXPORT bool computeAuth(const QString &actionId, qint64 pid = QCoreApplication::applicationPid());

/**
 * This function should be used by mechanisms (e.g.: helper applications).
 * It returns the action should be carried out, so if the caller was
 * actually authorized to perform it. The result is in form of a PolKitResult, so that
 * you can have more control over the whole process, and detect an eventual error.
 * Most of the times you simply want to check if the result is == to \c Result::Yes,
 * if you don't have specific needs.
 *
 * It is CRITICAL that you call this function
 * and check what it returns before doing anything in your helper, since otherwise
 * you could be actually performing an action from an unknown or unauthorized caller.
 *
 * The revokeIfOneShot parameter is very important: if it is set to true, if the authorization
 * was a OneShot authorization, it will be revoked right after calling this function.
 * To the bottom line, you should set this parameter to true if you're the mechanism
 * (so you're actually carrying out the action), to false if you're not (for example,
 * if you only want to check if the caller is authorized but not perform the action).
 *
 *
 * \note The \c pid parameter refers to the caller. You can retrieve this through
 *              DBus.
 *
 * \param actionId the Id of the action in question
 * \param pid the pid of the caller we want to check authorization for
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 *
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, qint64 pid);

/**
 * Same as above. Lets you use isCallerAuthorized with a QString DBus name instead of the
 * PID.
 *
 * \param actionId the Id of the action in question
 * \param dbusName unique name on the system message bus
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, const QString &dbusName);

/**
 * Same is other isCallerAuthorized but instead PID/DBus name requires caller subject.
 *
 * \param actionId the Id of the action in question
 * \param subject caller subject
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, PolkitSubject *subject);

/**
 * Registers an authentication agent.
 *
 * \param subject caller subject
 * \param locale the locale of the authentication agent
 * \param objectPath the object path for the authentication agent
 */
POLKIT_QT_EXPORT void registerAuthenticationAgent(PolkitSubject *subject, const QString &locale, 
						  const QString &objectPath);

void registerAuthenticationAgentCallback(GObject *source_object, GAsyncResult *res, gpointer user_data);
}

} 
 
#endif
