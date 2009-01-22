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

#include <QtCore>
#include <QtCore/QString>

namespace PolKitQt {

/**
 * \class PolkitQtAuth polkit_qt_auth.h PolkitQtAuth
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to obtain authorizations
 *
 * This class is a simple wrapper around the DBus interface
 * org.freedesktop.PolicyKit.AuthenticationAgent to make usage of
 * policykit easier in Qt/KDE worlds
 */
class PkAuth : public QObject
{
Q_OBJECT
public:
    PkAuth(QObject *parent = 0) : QObject(parent) {};

    /**
     * Obtain authorization for the given action regardless of
     * the Action Result
     * This method is meant to be used if you don't want to have
     * a PkAction class that can handle all states, it will compute
     * the Polkit Result and if needed prompt for the password.
     *
     * \p actionId is the action in question (ie org.freedesktop.policykit.read)
     * \p winId the X window id for the request (use 0 if there's no window)
     * \p pid Process id of the aplication in question
     * \return TRUE if the user is authorized otherwise FALSE
     */
    static bool computeAndObtainAuth(const QString &action_id, uint winId, uint pid = QCoreApplication::applicationPid());

    /**
     * Obtain authorization for the given action regardless of
     * the Action Result
     * This method was meant to be used only be PkAction but
     * you can use it as long as you know what you're doing
     * (ie. computing the Pokit Result for the given action first)
     Gets the files contained in the given \p packages
     *
     * \sa Transaction::files
     */
    static bool obtainAuth(const QString &action_id, uint xid, uint pid = QCoreApplication::applicationPid());
};

}

#endif
