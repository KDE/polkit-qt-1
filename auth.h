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

#include <QtCore/QString>
#include <QtCore/QCoreApplication>

namespace PolkitQt
{

/**
 * \class Auth auth.h Auth
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to obtain authorizations
 *
 * This class is a simple wrapper around the DBus interface
 * org.freedesktop.PolicyKit.AuthenticationAgent to make usage of
 * policykit easier in Qt/KDE worlds
 */
class POLKIT_QT_EXPORT Auth : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new Auth object
     *
     * \param parent the object parent
     */
    Auth(QObject *parent = 0);

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
    static bool computeAndObtainAuth(const QString &actionId, uint winId = 0, uint pid = QCoreApplication::applicationPid());

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
    static bool obtainAuth(const QString &actionId, uint winId = 0, uint pid = QCoreApplication::applicationPid());

    static bool isCallerAuthorized(const QString &actionId, uint pid, bool revokeIfOneShot);

private:
    class Private;
    Private * const d;
};

}

#endif
