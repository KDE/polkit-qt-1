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

#define PK_NAME      "org.freedesktop.PolicyKit.AuthenticationAgent"
#define PK_PATH      "/"
#define PK_INTERFACE "org.freedesktop.PolicyKit.AuthenticationAgent"

#include "polkit_qt_auth.h"
#include "polkit_qt_action.h"

#include <QtDBus/QtDBus>
#include <polkit-dbus/polkit-dbus.h>

using namespace QPolicyKit;

bool Auth::computeAndObtainAuth(const QString &actionId, uint winId, uint pid)
{
    PolKitAction *pkAction = polkit_action_new();
    polkit_action_set_action_id (pkAction, actionId.toAscii().data());
    PolKitResult result;
    result = Action::computePkResultDirect(pkAction, pid);
    switch (result) {
        case POLKIT_RESULT_YES:
            // If PolicyKit says yes.. emit the 'activated' signal
            return true;
            break;
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_SESSION:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_ALWAYS:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_SESSION:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_ALWAYS:
            // Otherwise, the action needs auth..
            // we now issue the obtain auth dialog
            if (pkAction != 0) {
                return obtainAuth(actionId, winId, pid);
            }
        default:
        case POLKIT_RESULT_NO:
            // if result is no then the user cannot even try to auth
            return false;
            break;
    }
}

bool Auth::obtainAuth(const QString &actionId, uint winId, uint pid)
{
    qDebug() << "obtainAuth" << actionId << winId << pid;
    QDBusMessage message = QDBusMessage::createMethodCall(PK_NAME, PK_PATH, PK_INTERFACE, QLatin1String("ObtainAuthorization"));
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(actionId) << qVariantFromValue(winId) << qVariantFromValue(pid);
    message.setArguments(argumentList);
    // Do a session bus call with BlockWithGui to create an event loop
    // and INT_MAX to get the highier timeout
    QDBusMessage reply = QDBusConnection::sessionBus().call(message, QDBus::BlockWithGui, INT_MAX);
    qDebug() << message.arguments();
    qDebug() << message.errorName();
    qDebug() << message.errorMessage();
    qDebug() << message.type();
    qDebug() << reply.arguments();
    qDebug() << reply.errorName();
    qDebug() << reply.errorMessage();
    qDebug() << reply.type();
    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().size() == 1) {
        return reply.arguments().first().toBool();
    } else if (reply.type() == QDBusMessage::MethodCallMessage) {
        qWarning() << "Message did not receive a reply (timeout by message bus)";
    }
    return false;
}

#include "polkit_qt_auth.moc"
