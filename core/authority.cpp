/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf@kde.org>
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
 * Copyright (C) 2009 Radek Novacek <rnovacek@redhat.com>
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

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <polkit/polkit.h>

namespace PolkitQt1
{

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

ActionDescription::List actionsToListAndFree(GList *glist)
{
    ActionDescription::List result;
    for (GList *glist2 = glist; glist2; glist2 = g_list_next(glist2)) {
        gpointer i = glist2->data;
        result.append(new ActionDescription(static_cast<PolkitActionDescription *>(i)));
        g_object_unref(i);
    }

    g_list_free(glist);
    return result;
}

class Authority::Private
{
public:
    // Polkit will return NULL on failures, hence we use it instead of 0
    Private(Authority *qq) : q(qq)
            , pkAuthority(NULL)
            , m_hasError(false) {}

    ~Private();

    void init();

    /** Use this method to set the error message to \p message. Set recover to \c true
     * to try to reinitialize this object with init() method
     */
    void setError(Authority::ErrorCode code, const QString &details = QString(), bool recover = false);

    void dbusFilter(const QDBusMessage &message);
    void dbusSignalAdd(const QString &service, const QString &path, const QString &interface, const QString &name);
    void seatSignalsConnect(const QString &seat);

    Authority *q;
    PolkitAuthority *pkAuthority;
    bool m_hasError;
    Authority::ErrorCode m_lastError;
    QString m_errorDetails;
    QDBusConnection *m_systemBus;
    GCancellable *m_checkAuthorizationCancellable,
    *m_enumerateActionsCancellable,
    *m_registerAuthenticationAgentCancellable,
    *m_unregisterAuthenticationAgentCancellable,
    *m_authenticationAgentResponseCancellable,
    *m_enumerateTemporaryAuthorizationsCancellable,
    *m_revokeTemporaryAuthorizationsCancellable,
    *m_revokeTemporaryAuthorizationCancellable;


    static void pk_config_changed();
    static void checkAuthorizationCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void enumerateActionsCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void registerAuthenticationAgentCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void unregisterAuthenticationAgentCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void authenticationAgentResponseCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void enumerateTemporaryAuthorizationsCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void revokeTemporaryAuthorizationsCallback(GObject *object, GAsyncResult *result, gpointer user_data);
    static void revokeTemporaryAuthorizationCallback(GObject *object, GAsyncResult *result, gpointer user_data);
};

Authority::Private::~Private()
{
    g_object_unref(m_checkAuthorizationCancellable);
    g_object_unref(m_enumerateActionsCancellable);
    g_object_unref(m_registerAuthenticationAgentCancellable);
    g_object_unref(m_unregisterAuthenticationAgentCancellable);
    g_object_unref(m_authenticationAgentResponseCancellable);
    g_object_unref(m_enumerateTemporaryAuthorizationsCancellable);
    g_object_unref(m_revokeTemporaryAuthorizationsCancellable);
    g_object_unref(m_revokeTemporaryAuthorizationCancellable);
}

Authority::Authority(PolkitAuthority *authority, QObject *parent)
        : QObject(parent)
        , d(new Private(this))
{
    qRegisterMetaType<PolkitQt1::Authority::Result> ();
    qRegisterMetaType<PolkitQt1::ActionDescription::List>();

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

    m_checkAuthorizationCancellable = g_cancellable_new();
    m_enumerateActionsCancellable = g_cancellable_new();
    m_registerAuthenticationAgentCancellable = g_cancellable_new();
    m_unregisterAuthenticationAgentCancellable = g_cancellable_new();
    m_authenticationAgentResponseCancellable = g_cancellable_new();
    m_enumerateTemporaryAuthorizationsCancellable = g_cancellable_new();
    m_revokeTemporaryAuthorizationsCancellable = g_cancellable_new();
    m_revokeTemporaryAuthorizationCancellable = g_cancellable_new();

    if (pkAuthority == NULL) {
        pkAuthority = polkit_authority_get();
    }

    if (pkAuthority == NULL) {
        (E_GetAuthority);
        return;
    }

    // connect changed signal
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
    foreach(const QString &seat, seats) {
        seatSignalsConnect(seat);
    }
}

void Authority::Private::setError(Authority::ErrorCode code, const QString &details, bool recover)
{
    if (recover)
        init();
    m_lastError = code;
    m_errorDetails = details;
    m_hasError = true;
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
    if (message.type() == QDBusMessage::SignalMessage) {
        emit q->consoleKitDBChanged();

        // TODO: Test this with the multiseat support
        if (message.member() == "SeatAdded")
            seatSignalsConnect(qVariantValue<QDBusObjectPath> (message.arguments()[0]).path());
    }
}

bool Authority::hasError() const
{
    return d->m_hasError;
}

Authority::ErrorCode Authority::lastError() const
{
    return d->m_lastError;
}

const QString Authority::errorDetails() const
{
    if (d->m_lastError == E_None)
        return QString();
    else
        return d->m_errorDetails;
}

void Authority::clearError()
{
    d->m_hasError = false;
    d->m_lastError = E_None;
}

void Authority::Private::pk_config_changed()
{
    emit Authority::instance()->configChanged();
}

PolkitAuthority *Authority::polkitAuthority() const
{
    return d->pkAuthority;
}

Authority::Result Authority::checkAuthorizationSync(const QString &actionId, Subject *subject, AuthorizationFlags flags)
{
    PolkitAuthorizationResult *pk_result;
    GError *error = NULL;

    if (Authority::instance()->hasError()) {
        return Unknown;
    }

    if (subject == NULL) {
        d->setError(E_WrongSubject);
        return Unknown;
    }

    pk_result = polkit_authority_check_authorization_sync(d->pkAuthority,
                subject->subject(),
                actionId.toAscii().data(),
                NULL,
                (PolkitCheckAuthorizationFlags)(int)flags,
                NULL,
                &error);

    if (error != NULL) {
        d->setError(E_CheckFailed, error->message);
        g_error_free(error);
        return Unknown;
    }

    if (!pk_result) {
        d->setError(E_UnknownResult);
        return Unknown;
    } else {
        Authority::Result res = polkitResultToResult(pk_result);
        g_object_unref(pk_result);
        return res;
    }
}

void Authority::checkAuthorization(const QString &actionId, Subject *subject, AuthorizationFlags flags)
{
    if (Authority::instance()->hasError()) {
        return;
    }

    if (subject == NULL) {
        d->setError(E_WrongSubject);
        return;
    }

    polkit_authority_check_authorization(d->pkAuthority,
                                         subject->subject(),
                                         actionId.toAscii().data(),
                                         NULL,
                                         (PolkitCheckAuthorizationFlags)(int)flags,
                                         d->m_checkAuthorizationCancellable,
                                         d->checkAuthorizationCallback, this);
}

void Authority::Private::checkAuthorizationCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;

    Q_ASSERT(authority != NULL);

    GError *error = NULL;
    PolkitAuthorizationResult *pkResult = polkit_authority_check_authorization_finish((PolkitAuthority *) object, result, &error);

    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_CheckFailed, error->message);
        g_error_free(error);
        return;
    }
    if (pkResult != NULL) {
        emit authority->checkAuthorizationFinished(polkitResultToResult(pkResult));
        g_object_unref(pkResult);
    } else {
        authority->d->setError(E_UnknownResult);
    }
}

void Authority::checkAuthorizationCancel()
{
    if (!g_cancellable_is_cancelled(d->m_checkAuthorizationCancellable))
        g_cancellable_cancel(d->m_checkAuthorizationCancellable);
}

ActionDescription::List Authority::enumerateActionsSync()
{
    if (Authority::instance()->hasError()) {
        return ActionDescription::List();
    }

    GError *error = NULL;

    GList *glist = polkit_authority_enumerate_actions_sync(d->pkAuthority,
                   NULL,
                   &error);

    if (error != NULL) {
        d->setError(E_EnumFailed, error->message);
        g_error_free(error);
        return ActionDescription::List();
    }

    return actionsToListAndFree(glist);
}

void Authority::enumerateActions()
{
    if (Authority::instance()->hasError()) {
        return;
    }

    polkit_authority_enumerate_actions(d->pkAuthority,
                                       d->m_enumerateActionsCancellable,
                                       d->enumerateActionsCallback,
                                       Authority::instance());
}

void Authority::Private::enumerateActionsCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority != NULL);
    GError *error = NULL;
    GList *list = polkit_authority_enumerate_actions_finish((PolkitAuthority *) object, result, &error);
    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_EnumFailed, error->message);
        g_error_free(error);
        return;
    }

    emit authority->enumerateActionsFinished(actionsToListAndFree(list));
}

void Authority::enumerateActionsCancel()
{
    if (!g_cancellable_is_cancelled(d->m_enumerateActionsCancellable))
        g_cancellable_cancel(d->m_enumerateActionsCancellable);
}

bool Authority::registerAuthenticationAgentSync(Subject *subject, const QString &locale, const QString &objectPath)
{
    if (Authority::instance()->hasError()) {
        return false;
    }

    gboolean result;
    GError *error = NULL;

    if (!subject) {
        d->setError(E_WrongSubject);
        return false;
    }

    result = polkit_authority_register_authentication_agent_sync(d->pkAuthority,
             subject->subject(), locale.toAscii().data(),
             objectPath.toAscii().data(), NULL, &error);

    if (error) {
        d->setError(E_RegisterFailed, error->message);
        g_error_free(error);
        return false;
    }

    return result;
}

void Authority::registerAuthenticationAgent(Subject *subject, const QString &locale, const QString &objectPath)
{
    if (Authority::instance()->hasError()) {
        return;
    }

    if (!subject) {
        d->setError(E_WrongSubject);
        return;
    }

    polkit_authority_register_authentication_agent(d->pkAuthority,
            subject->subject(),
            locale.toAscii().data(),
            objectPath.toAscii().data(),
            d->m_registerAuthenticationAgentCancellable,
            d->registerAuthenticationAgentCallback,
            this);
}

void Authority::Private::registerAuthenticationAgentCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority != NULL);
    GError *error = NULL;
    bool res = polkit_authority_register_authentication_agent_finish((PolkitAuthority *) object, result, &error);
    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_EnumFailed , error->message);
        g_error_free(error);
        return;
    }

    emit authority->registerAuthenticationAgentFinished(res);
}

void Authority::registerAuthenticationAgentCancel()
{
    if (!g_cancellable_is_cancelled(d->m_registerAuthenticationAgentCancellable))
        g_cancellable_cancel(d->m_registerAuthenticationAgentCancellable);
}

bool Authority::unregisterAuthenticationAgentSync(Subject *subject, const QString &objectPath)
{
    if (d->pkAuthority)
        return false;

    if (!subject) {
        d->setError(E_WrongSubject);
        return false;
    }

    GError *error = NULL;

    bool result = polkit_authority_unregister_authentication_agent_sync(d->pkAuthority,
                  subject->subject(),
                  objectPath.toUtf8().data(),
                  NULL,
                  &error);

    if (error != NULL) {
        d->setError(E_UnregisterFailed, error->message);
        g_error_free(error);
        return false;
    }

    return result;
}

void Authority::unregisterAuthenticationAgent(Subject *subject, const QString &objectPath)
{
    if (Authority::instance()->hasError())
        return;

    if (!subject) {
        d->setError(E_WrongSubject);
        return;
    }

    polkit_authority_unregister_authentication_agent(d->pkAuthority,
            subject->subject(),
            objectPath.toUtf8().data(),
            d->m_unregisterAuthenticationAgentCancellable,
            d->unregisterAuthenticationAgentCallback,
            this);
}

void Authority::Private::unregisterAuthenticationAgentCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority);
    GError *error = NULL;
    bool res = polkit_authority_unregister_authentication_agent_finish((PolkitAuthority *) object, result, &error);
    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_UnregisterFailed, error->message);
        g_error_free(error);
        return;
    }

    emit authority->unregisterAuthenticationAgentFinished(res);
}

void Authority::unregisterAuthenticationAgentCancel()
{
    if (!g_cancellable_is_cancelled(d->m_unregisterAuthenticationAgentCancellable))
        g_cancellable_cancel(d->m_unregisterAuthenticationAgentCancellable);
}

bool Authority::authenticationAgentResponseSync(const QString &cookie, Identity *identity)
{
    if (Authority::instance()->hasError())
        return false;

    if (cookie.isEmpty() || !identity) {
        d->setError(E_CookieOrIdentityEmpty);
        return false;
    }

    GError *error = NULL;

    bool result = polkit_authority_authentication_agent_response_sync(d->pkAuthority,
                  cookie.toUtf8().data(),
                  identity->identity(),
                  NULL,
                  &error);
    if (error != NULL) {
        d->setError(E_AgentResponseFailed, error->message);
        g_error_free(error);
        return false;
    }

    return result;
}

void Authority::authenticationAgentResponse(const QString &cookie, Identity *identity)
{
    if (Authority::instance()->hasError())
        return;

    if (cookie.isEmpty() || !identity) {
        d->setError(E_CookieOrIdentityEmpty);
        return;
    }

    polkit_authority_authentication_agent_response(d->pkAuthority,
            cookie.toUtf8().data(),
            identity->identity(),
            d->m_authenticationAgentResponseCancellable,
            d->authenticationAgentResponseCallback,
            this);
}

void Authority::Private::authenticationAgentResponseCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority);
    GError *error = NULL;
    bool res = polkit_authority_authentication_agent_response_finish((PolkitAuthority *) object, result, &error);
    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_AgentResponseFailed, error->message);
        g_error_free(error);
        return;
    }

    emit authority->authenticationAgentResponseFinished(res);
}

void Authority::authenticationAgentResponseCancel()
{
    if (!g_cancellable_is_cancelled(d->m_authenticationAgentResponseCancellable))
        g_cancellable_cancel(d->m_authenticationAgentResponseCancellable);
}

QList<TemporaryAuthorization *> Authority::enumerateTemporaryAuthorizationsSync(Subject *subject)
{
    QList<TemporaryAuthorization *> result;

    GError *error = NULL;
    GList *glist = polkit_authority_enumerate_temporary_authorizations_sync(d->pkAuthority,
                   subject->subject(),
                   NULL,
                   &error);
    if (error != NULL) {
        d->setError(E_EnumFailed, error->message);
        g_error_free(error);
        return result;
    }

    GList *glist2;
    for (glist2 = glist; glist2 != NULL; glist2 = g_list_next(glist2)) {
        result.append(new TemporaryAuthorization((PolkitTemporaryAuthorization *) glist2->data));
        g_object_unref(glist2->data);
    }

    g_list_free(glist);

    return result;
}

void Authority::Private::enumerateTemporaryAuthorizationsCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority);
    GError *error = NULL;

    GList *glist = polkit_authority_enumerate_temporary_authorizations_finish((PolkitAuthority *) object, result, &error);

    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_EnumFailed, error->message);
        g_error_free(error);
        return;
    }
    QList<TemporaryAuthorization *> res;
    GList *glist2;
    for (glist2 = glist; glist2 != NULL; glist2 = g_list_next(glist2)) {
        res.append(new TemporaryAuthorization((PolkitTemporaryAuthorization *) glist2->data));
        g_object_unref(glist2->data);
    }

    g_list_free(glist);

    emit authority->enumerateTemporaryAuthorizationsFinished(res);
}

void Authority::enumerateTemporaryAuthorizationsCancel()
{
    if (!g_cancellable_is_cancelled(d->m_enumerateTemporaryAuthorizationsCancellable))
        g_cancellable_cancel(d->m_enumerateTemporaryAuthorizationsCancellable);
}

bool Authority::revokeTemporaryAuthorizationsSync(Subject *subject)
{
    bool result;
    if (Authority::instance()->hasError())
        return false;

    GError *error = NULL;
    result = polkit_authority_revoke_temporary_authorizations_sync(d->pkAuthority,
             subject->subject(),
             NULL,
             &error);
    if (error != NULL) {
        d->setError(E_RevokeFailed, error->message);
        g_error_free(error);
        return false;
    }
    return result;
}

void Authority::revokeTemporaryAuthorizations(Subject *subject)
{
    if (Authority::instance()->hasError())
        return;

    polkit_authority_revoke_temporary_authorizations(d->pkAuthority,
            subject->subject(),
            d->m_revokeTemporaryAuthorizationsCancellable,
            d->revokeTemporaryAuthorizationsCallback,
            this);
}

void Authority::Private::revokeTemporaryAuthorizationsCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority != NULL);
    GError *error = NULL;

    bool res = polkit_authority_revoke_temporary_authorizations_finish((PolkitAuthority *) object, result, &error);

    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_RevokeFailed, error->message);
        g_error_free(error);
        return;
    }

    emit authority->revokeTemporaryAuthorizationsFinished(res);
}

void Authority::revokeTemporaryAuthorizationsCancel()
{
    if (!g_cancellable_is_cancelled(d->m_revokeTemporaryAuthorizationsCancellable))
        g_cancellable_cancel(d->m_revokeTemporaryAuthorizationsCancellable);
}

bool Authority::revokeTemporaryAuthorizationSync(const QString &id)
{
    bool result;
    if (Authority::instance()->hasError())
        return false;

    GError *error = NULL;
    result =  polkit_authority_revoke_temporary_authorization_by_id_sync(d->pkAuthority,
              id.toUtf8().data(),
              NULL,
              &error);
    if (error != NULL) {
        d->setError(E_RevokeFailed, error->message);
        g_error_free(error);
        return false;
    }
    return result;
}

void Authority::revokeTemporaryAuthorization(const QString &id)
{
    if (Authority::instance()->hasError())
        return;

    polkit_authority_revoke_temporary_authorization_by_id(d->pkAuthority,
            id.toUtf8().data(),
            d->m_revokeTemporaryAuthorizationCancellable,
            d->revokeTemporaryAuthorizationCallback,
            this);
}

void Authority::Private::revokeTemporaryAuthorizationCallback(GObject *object, GAsyncResult *result, gpointer user_data)
{
    Authority *authority = (Authority *) user_data;
    Q_ASSERT(authority != NULL);
    GError *error = NULL;

    bool res = polkit_authority_revoke_temporary_authorization_by_id_finish((PolkitAuthority *) object, result, &error);

    if (error != NULL) {
        // We don't want to set error if this is cancellation of some action
        if (error->code != 1)
            authority->d->setError(E_RevokeFailed, error->message);
        g_error_free(error);
        return;
    }

    emit authority->revokeTemporaryAuthorizationFinished(res);
}

void Authority::revokeTemporaryAuthorizationCancel()
{
    if (!g_cancellable_is_cancelled(d->m_revokeTemporaryAuthorizationCancellable))
        g_cancellable_cancel(d->m_revokeTemporaryAuthorizationCancellable);
}

}

#include "authority.moc"
