/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
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

#include <polkit/polkit.h>

#include <QtCore/QString>
#include <QtCore/QCoreApplication>

namespace PolkitQt
{

/**
 * \class Auth auth.h Auth
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
    POLKIT_QT_EXPORT bool computeAndObtainAuth(const QString &actionId, uint winId = 0, uint pid = QCoreApplication::applicationPid());

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
    POLKIT_QT_EXPORT bool obtainAuth(const QString &actionId, uint winId = 0, uint pid = QCoreApplication::applicationPid());

    /**
     * This function should be used by mechanisms (e.g.: helper applications).
     * It returns the action should be carried out, so if the caller was
     * actually authorized to perform it. The result is in form of a PolKitResult, so that
     * you can have more control over the whole process, and detect an eventual error.
     * Most of the times you simply want to check if the result is == to \c POLKIT_RESULT_YES,
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
     * \note The \c pid parameter refers to the caller. You can retrieve this through
     *              DBus.
     *
     * \param actionId the Id of the action in question
     * \param pid the pid of the caller we want to check authorization for
     * \param revokeIfOneShot \c true  if we're carrying out the action, so we want the auth
     *                                 to be revoked right after
     *                        \c false if we're not carrying out the action, so we don't want
     *                                 the auth to be revoked right after
     *
     * \return \c POLKIT_RESULT_YES if the caller is authorized and the action should be performed
     *         \c otherwise if the caller was not authorized and the action should not be performed,
     *                      or an error has occurred
     *
     */
    POLKIT_QT_EXPORT PolKitResult isCallerAuthorized(const QString &actionId, uint pid, bool revokeIfOneShot);

    /**
     * Convenience overload. Lets you use isCallerAuthorized with a PolKitAction instead of a simple
     * action Id.
     *
     * \param action the PolKitAction in question
     * \param pid the pid of the caller we want to check authorization for
     * \param revokeIfOneShot \c true  if we're carrying out the action, so we want the auth
     *                                 to be revoked right after
     *                        \c false if we're not carrying out the action, so we don't want
     *                                 the auth to be revoked right after
     *
     * \return \c POLKIT_RESULT_YES if the caller is authorized and the action should be performed
     *         \c otherwise if the caller was not authorized and the action should not be performed,
     *                      or an error has occurred
     */
    POLKIT_QT_EXPORT PolKitResult isCallerAuthorized(PolKitAction *action, pid_t pid, bool revokeIfOneShot);
}

}

#endif
