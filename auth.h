/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
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
#include <QtGui/qwindowdefs.h>

typedef struct _PolKitAction PolKitAction;

namespace PolkitQt
{

/**
 * \namespace
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
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
    /** The action can be performed */
    Yes = 0x01,
    /** The user needs to authenticate as admin.
     *  The authentication will be immediately released.
     */
    AdminAuthOneShot = 0x02,
    /** The user needs to authenticate as admin */
    AdminAuth = 0x03,
    /** The user needs to authenticate as admin.
     *  The authentication will be kept during this session.
     */
    AdminAuthKeepSession = 0x04,
    /** The user needs to authenticate as admin.
     *  The authentication will be kept indefinitely.
     */
    AdminAuthKeepAlways = 0x5,
    /** The user needs to authenticate as himself.
     *  The authentication will be immediately released.
     */
    SelfAuthOneShot = 0x6,
    /** The user needs to authenticate as himself */
    SelfAuth = 0x7,
    /** The user needs to authenticate as himself.
     *  The authentication will be kept during this session.
     */
    SelfAuthKeepSession = 0x8,
    /** The user needs to authenticate as himself.
     *  The authentication will be kept indefinitely.
     */
    SelfAuthKeepAlways = 0x9,
    /** The action can't be performed */
    No = 0x10
};

/**
 * Obtains authorization for the given action regardless of
 * the Action Result.
 * This method is meant to be used if you don't want to have
 * an Action class that can handle all states. It will simply
 * compute the Polkit Result and prompt for password if needed.
 *
 * \see Action
 *
 * \param actionId id of the action in question (i.e. org.freedesktop.policykit.read)
 * \param winId the X window id for the request (use 0 if there's no window)
 * \param pid Process id of the application in question
 * \return \c true if the user is authorized
 *         \c false if the user is not authorized
 */
POLKIT_QT_EXPORT bool computeAndObtainAuth(const QString &actionId, WId winId = 0, qint64 pid = QCoreApplication::applicationPid());

/**
 * Obtain authorization for the given action regardless of
 * the Action Result.
 * This method was meant to be used only by Action,
 * use it only if you know what you're doing (ie. computing
 * the Polkit Result for the given action first). If you are unsure,
 * use computeAndObtainAuth instead
 *
 * \see computeAndObtainAuth
 *
 * \param actionId id of the action in question (i.e. org.freedesktop.policykit.read)
 * \param winId the X window id for the request (use 0 if there's no window)
 * \param pid Process id of the application in question
 * \return \c true if the user is authorized
 *         \c false if the user is not authorized
 */
POLKIT_QT_EXPORT bool obtainAuth(const QString &actionId, WId winId = 0, qint64 pid = QCoreApplication::applicationPid());

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
 * DEPRECATION WARNING: This function uses a type that will be removed in
 * PolicyKit 1.0. Please don't use this function in your application unless strictly
 * needed, or you will have to update it when polkit-qt 1.0 will come out. Please use
 * the equivalent without PolKit* types.
 *
 * \note The \c pid parameter refers to the caller. You can retrieve this through
 *              DBus.
 *
 * \param action the PolKitAction in question
 * \param pid the pid of the caller we want to check authorization for
 * \param revokeIfOneShot \c true  if we're carrying out the action, so we want the auth
 *                                 to be revoked right after
 *                        \c false if we're not carrying out the action, so we don't want
 *                                 the auth to be revoked right after
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 *
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(PolKitAction *action, qint64 pid, bool revokeIfOneShot);

/**
 * Convenience overload. Lets you use isCallerAuthorized with a QString instead of a PolKitAction.
 *
 * \param actionId the Id of the action in question
 * \param pid the pid of the caller we want to check authorization for
 * \param revokeIfOneShot \c true  if we're carrying out the action, so we want the auth
 *                                 to be revoked right after
 *                        \c false if we're not carrying out the action, so we don't want
 *                                 the auth to be revoked right after
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, qint64 pid, bool revokeIfOneShot);

/**
 * Same as above. Lets you use isCallerAuthorized with a QString DBus name instead of the
 * PID.
 *
 * \param actionId the Id of the action in question
 * \param dbusName unique name on the system message bus
 * \param revokeIfOneShot \c true  if we're carrying out the action, so we want the auth
 *                                 to be revoked right after
 *                        \c false if we're not carrying out the action, so we don't want
 *                                 the auth to be revoked right after
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(const QString &actionId, const QString &dbusName, bool revokeIfOneShot);

/**
 * Same as above. Lets you use isCallerAuthorized with a QString DBus name instead of the
 * PID.
 *
 * DEPRECATION WARNING: This function uses a type that will be removed in
 * PolicyKit 1.0. Please don't use this function in your application unless strictly
 * needed, or you will have to update it when polkit-qt 1.0 will come out. Please use
 * the equivalent without PolKit* types.
 *
 * \param action the PolKitAction in question
 * \param dbusName unique name on the system message bus
 * \param revokeIfOneShot \c true  if we're carrying out the action, so we want the auth
 *                                 to be revoked right after
 *                        \c false if we're not carrying out the action, so we don't want
 *                                 the auth to be revoked right after
 *
 * \return \c Result::Yes if the caller is authorized and the action should be performed
 *         \c otherwise if the caller was not authorized and the action should not be performed,
 *                      or an error has occurred
 */
POLKIT_QT_EXPORT Result isCallerAuthorized(PolKitAction *action, const QString &dbusName, bool revokeIfOneShot);
}

}

#endif
