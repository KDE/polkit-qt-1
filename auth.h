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
#include <QtCore/QFlags>
#include <polkit/polkit.h>

namespace PolkitQt
{

/**
 * \namespace
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf54321@gmail.com>
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief Functions used to obtain authorizations
 *
 * This namespace contains class is a simple wrapper around...
 * TODO:
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

enum AuthorizationFlag {
    /** No flags set **/
    None = 0x00,
    /** If the subject can obtain the authorization through authentication, 
    * and an authentication agent is available, then attempt to do so. 
    * 
    * Note, this means that the method used for checking authorization is likely 
    * to block for a long time. **/
    AllowUserInteraction = 0x01
};

Q_DECLARE_FLAGS(AuthorizationFlags, AuthorizationFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(AuthorizationFlags)

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
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, qint64 pid, 
					   AuthorizationFlags flags);

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
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, const QString &dbusName,
					   AuthorizationFlags flags);

/**
 * Same as above but instead PID/DBus name requires caller PolicyKit subject.
 *
 * \param actionId the Id of the action in question
 * \param subject caller subject
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, PolkitSubject *subject,
					   AuthorizationFlags flags);

/**
  * Synchronously retrieves all registered actions.
  *
  * \return a list of Action IDs
  */
POLKIT_QT_EXPORT QStringList enumerateActions();


/**
 * Registers an Authentication agent.
 *
 * \param pid caller subject
 * \param locale the locale of the Authentication agent
 * \param objectPath the object path for the Authentication agent
 *
 * \return \c true if the Authentication agent has been successfully registered
 *         \c false if the Authentication agent registration failed
*/
POLKIT_QT_EXPORT bool registerAuthenticationAgent(qint64 pid, const QString &locale, 
						  const QString &objectPath);

/**
 * Registers an authentication agent.
 *
 * \param subject caller subject
 * \param locale the locale of the authentication agent
 * \param objectPath the object path for the authentication agent
 *
 * \return \c true if the Authentication agent has been successfully registered
 *         \c false if the Authentication agent registration failed
*/
POLKIT_QT_EXPORT bool registerAuthenticationAgent(PolkitSubject *subject, const QString &locale, 
						  const QString &objectPath);

}

} 
 
#endif
