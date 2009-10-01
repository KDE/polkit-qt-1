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

#include "authority.h"

#include <QtCore/QMap>
#include <QtCore/QSocketNotifier>
#include <QtCore/QStringList>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtCore/QDebug>
#include <QtCore/QGlobalStatic>
#include <QtXml/QDomDocument>

#include <polkit/polkit.h>
#include <glib-object.h>

using namespace PolkitQt;

class AuthorityHelper
{
public:
    AuthorityHelper() : q(0) {}
    ~AuthorityHelper() {
        delete q;
    }
    Authority *q;
};

Q_GLOBAL_STATIC(AuthorityHelper, s_globalAuthority)

Authority *Authority::instance(PolkitAuthority *authority)
{
    if (!s_globalAuthority()->q) {
        new Authority(authority);
    }

    return s_globalAuthority()->q;
}

Authority::Result polkitResultToResult(PolkitAuthorizationResult *result)
{
    if (polkit_authorization_result_get_is_challenge(result))
        return Authority::Challenge;
    else if (polkit_authorization_result_get_is_authorized(result))
        return Authority::Yes;
    else
        return Authority::No;
}

class Authority::Private
{
public:
    // Polkit will return NULL on failures, hence we use it instead of 0
    Private(Authority *qq) : q(qq)
            , pkAuthority(NULL)
            , m_hasError(false) {}

    void init();
    void dbusFilter(const QDBusMessage &message);
    void dbusSignalAdd(const QString &service, const QString &path, const QString &interface, const QString &name);
    void seatSignalsConnect(const QString &seat);

    Authority *q;
    PolkitAuthority *pkAuthority;
    bool m_hasError;
    QString m_lastError;
    QDBusConnection *m_systemBus;

    static void pk_config_changed();
};

Authority::Authority(PolkitAuthority *authority, QObject *parent)
        : QObject(parent)
        , d(new Private(this))
{
    Q_ASSERT(!s_globalAuthority()->q);
    s_globalAuthority()->q = this;

    if (authority) {
        d->pkAuthority = authority;
    }

    d->init();
}

Authority::~Authority()
{
    if (d->pkAuthority != NULL) {
        g_object_unref(d->pkAuthority);
    }

    delete d;
}

void Authority::Private::init()
{
    QDBusError error;
    QDBusError dbus_error;

    g_type_init();

    if (pkAuthority == NULL) {
        pkAuthority = polkit_authority_get();
    }

    if (pkAuthority == NULL)
        qWarning("Can't get authority authority!");

    // connect changed signal
    // TODO: test it first!
    g_signal_connect(G_OBJECT(pkAuthority), "changed", G_CALLBACK(pk_config_changed), NULL);
    
    // need to listen to NameOwnerChanged
    dbusSignalAdd("org.freedesktop.DBus", "/", "org.freedesktop.DBus", "NameOwnerChanged");

    QString consoleKitService("org.freedesktop.ConsoleKit");
    QString consoleKitManagerPath("/org/freedesktop/ConsoleKit/Manager");
    QString consoleKitManagerInterface("org.freedesktop.ConsoleKit.Manager");
    QString consoleKitSeatInterface("org.freedesktop.ConsoleKit.Seat");

    // first, add signals SeadAdded and SeatRemoved from ConsoleKit Manager
    dbusSignalAdd(consoleKitService, consoleKitManagerPath, consoleKitManagerInterface, "SeatAdded");
    dbusSignalAdd(consoleKitService, consoleKitManagerPath, consoleKitManagerInterface, "SeatRemoved");

    // then we need to extract all seats from ConsoleKit
    QDBusMessage msg = QDBusMessage::createMethodCall(consoleKitService, consoleKitManagerPath, consoleKitManagerInterface, "GetSeats");
    msg = QDBusConnection::systemBus().call(msg);
    // this method returns a list with present seats
    QList<QString> seats;
    qVariantValue<QDBusArgument> (msg.arguments()[0]) >> seats;
    // it can be multiple seats present so connect all their signals
    foreach (QString seat, seats)
    {
        seatSignalsConnect(seat);
    }
}

void Authority::Private::seatSignalsConnect(const QString &seat)
{
    QString consoleKitService("org.freedesktop.ConsoleKit");
    QString consoleKitSeatInterface("org.freedesktop.ConsoleKit.Seat");
    // we want to connect to all slots of the seat
    dbusSignalAdd(consoleKitService, seat, consoleKitSeatInterface, "DeviceAdded");
    dbusSignalAdd(consoleKitService, seat, consoleKitSeatInterface, "DeviceRemoved");
    dbusSignalAdd(consoleKitService, seat, consoleKitSeatInterface, "SessionAdded");
    dbusSignalAdd(consoleKitService, seat, consoleKitSeatInterface, "SessionRemoved");
    dbusSignalAdd(consoleKitService, seat, consoleKitSeatInterface, "ActiveSessionChanged");
}

void Authority::Private::dbusSignalAdd(const QString &service, const QString &path, const QString &interface, const QString &name)
{
    // FIXME: This code seems to be nonfunctional - it needs to be fixed somewhere (is it Qt BUG?)
    QDBusConnection::systemBus().connect(service, path, interface, name,
            q, SLOT(dbusFilter(const QDBusMessage &)));
}

void Authority::Private::dbusFilter(const QDBusMessage &message)
{
    if (message.type() == QDBusMessage::SignalMessage)
    {
        qDebug() << "INCOMING SIGNAL:" << message.member();
        emit q->consoleKitDBChanged();

        // TODO: Test this with the multiseat support
        if (message.member() == "SeatAdded")
            seatSignalsConnect(qVariantValue<QDBusObjectPath> (message.arguments()[0]).path());
    }
}

bool Authority::hasError() const
{
    if (d->m_hasError) {
        // try init again maybe something get
        // back to normal (as DBus might restarted, crashed...)
        d->init();
    }
    return d->m_hasError;
}

QString Authority::lastError() const
{
    return d->m_lastError;
}

void Authority::Private::pk_config_changed()
{
    emit Authority::instance()->configChanged();
}

PolkitAuthority *Authority::getPolkitAuthority() const
{
    return d->pkAuthority;
}

Authority::Result Authority::checkAuthorization(const QString &actionId, Subject *subject, AuthorizationFlags flags)
{
    PolkitAuthorizationResult *pk_result;
    GError *error = NULL;

    if (Authority::instance()->hasError()) {
        return Unknown;
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
        return Unknown;
    }

    if (!pk_result)
        return Unknown;
    else
        return polkitResultToResult(pk_result);
}

QStringList Authority::enumerateActions()
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

    QStringList result;
    GList * glist2;
    for (glist2 = glist; glist2; glist2 = g_list_next(glist2))
    {
        gpointer i = glist2->data;
        result.append(QString::fromUtf8(polkit_action_description_get_action_id((PolkitActionDescription*)i)));
        g_object_unref(i);
    }

    g_list_free(glist);

    return result;
}

bool Authority::registerAuthenticationAgent(Subject *subject, const QString &locale, const QString &objectPath)
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

bool Authority::unregisterAuthenticationAgent(Subject *subject, const QString &objectPath)
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

bool Authority::authenticationAgentResponse(const QString & cookie, Identity * identity)
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

QList<TemporaryAuthorization *> Authority::enumerateTemporaryAuthorizations(Subject *subject)
{
    QList<TemporaryAuthorization *> result;
    if (Authority::instance()->hasError())
        return result;

    GError *error = NULL;
    GList *glist = polkit_authority_enumerate_temporary_authorizations_sync(Authority::instance()->getPolkitAuthority(),
                                                                            subject->subject(),
                                                                            NULL,
                                                                            &error);
    if (error != NULL)
    {
        qWarning("Enumerate termporary actions failed with: %s", error->message);
        g_error_free(error);
        return result;
    }

    GList *glist2;
    for (glist2 = glist; glist2 != NULL; glist2 = g_list_next(glist2))
    {
        result.append(new TemporaryAuthorization((PolkitTemporaryAuthorization *) glist2->data));
        g_object_unref(glist2->data);
    }

    g_list_free(glist);

    return result;
}

bool Authority::revokeTemporaryAuthorizations(Subject *subject)
{
    bool result;
    if (Authority::instance()->hasError())
        return false;

    GError *error = NULL;
    result = polkit_authority_revoke_temporary_authorizations_sync(Authority::instance()->getPolkitAuthority(),
                                                                   subject->subject(),
                                                                   NULL,
                                                                   &error);
    if (error != NULL)
    {
        qWarning("Revoke temporary actions failed with: %s", error->message);
        g_error_free(error);
        return false;
    }
    return result;
}

bool Authority::revokeTemporaryAuthorization(const QString &id)
{
    bool result;
    if (Authority::instance()->hasError())
        return false;

    GError *error = NULL;
    result =  polkit_authority_revoke_temporary_authorization_by_id_sync(Authority::instance()->getPolkitAuthority(),
                                                                         id.toUtf8().data(),
                                                                         NULL,
                                                                         &error);
    if (error != NULL)
    {
        qWarning("Revoke temporary actions failed with: %s", error->message);
        g_error_free(error);
        return false;
    }
    return result;
}

#include "moc_authority.cpp"
