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

#include "polkit_qt_context.h"

#include <QtCore/QSocketNotifier>
#include <QtCore/QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QtCore/QDebug>

#include <polkit-dbus/polkit-dbus.h>

using namespace QPolicyKit;

Context* Context::m_self = 0;

Context* Context::instance()
{
    if(!m_self)
        new Context(qApp);

    return m_self;
}

// I'm using null instead of 0 as polkit will return
// NULL on failures
Context::Context(QObject *parent)
 : QObject(parent), pkContext(NULL),
   pkTracker(NULL), m_hasError(false)
{
    Q_ASSERT(!m_self);
    m_self = this;

    qDebug() << "Context - Constructing singleton";
    init();
}

Context::~Context()
{
    if (pkContext != NULL) {
        polkit_context_unref(pkContext);
    }
    if (pkTracker != NULL) {
        polkit_tracker_unref(pkTracker);
    }
}

void Context::init()
{
        DBusError error;
        DBusError dbus_error;
        PolKitError *pk_error;
dbus_error_init(&error);
//         if ((_singleton->priv->system_bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, error)) == NULL) {
//                 goto error;
//         }

if ((m_systemBus = dbus_bus_get(DBUS_BUS_SYSTEM, &error )) == NULL) {
qWarning() << "Failed to initialize DBus";
        }

    pkContext = polkit_context_new ();
    polkit_context_set_io_watch_functions (pkContext, io_add_watch, io_remove_watch);
    polkit_context_set_config_changed (pkContext, pk_config_changed, this);

    pk_error = NULL;
    if (!polkit_context_init (pkContext, &pk_error)) {
        qWarning() << "Failed to initialize PolicyKit context: " << polkit_error_get_error_message (pk_error);
        m_lastError = polkit_error_get_error_message (pk_error);
        m_hasError  = true;
        if (pkContext != NULL) {
            polkit_context_unref(pkContext);
        }
        polkit_error_free (pk_error);
        return;
    }
        /* TODO FIXME: I'm pretty sure dbus-glib blows in a way that
         * we can't say we're interested in all signals from all
         * members on all interfaces for a given service... So we do
         * this..
         */

        dbus_error_init (&dbus_error);
//
        /* need to listen to NameOwnerChanged */
//         dbus_bus_add_match (m_systemBus,
//                             "type='signal'"
//                             ",interface='"DBUS_INTERFACE_DBUS"'"
//                             ",sender='"DBUS_SERVICE_DBUS"'"
//                             ",member='NameOwnerChanged'",
//                             &dbus_error);
 if (QDBusConnection::systemBus().connect( DBUS_SERVICE_DBUS, QString(), DBUS_INTERFACE_DBUS, "NameOwnerChanged", this, SLOT(dbusFilter(const QDBusMessage &)))) {
qWarning() << "---------------------OK";
 } else {
qWarning() << "---------------------not Ok";
 }

//         if (dbus_error_is_set (&dbus_error)) {
// //                 dbus_set_g_error (error, &dbus_error);
//                 dbus_error_free (&dbus_error);
//                 m_hasError  = true;
//                 qWarning() << "Failed to initialize NameOwnerChanged";
//                 return;
// //                 goto error;
//         }
//

 if (QDBusConnection::systemBus().connect( "org.freedesktop.ConsoleKit", QString(), "org.freedesktop.ConsoleKit", QString(), this, SLOT(dbusFilter(const QDBusMessage &)))) {
qWarning() << "---------------------OK";
 } else {
qWarning() << "---------------------not Ok";
 }
        /* need to listen to ConsoleKit signals */
//         dbus_bus_add_match (m_systemBus,
//                             "type='signal',sender='org.freedesktop.ConsoleKit'",
//                             &dbus_error);
//
        if (dbus_error_is_set (&dbus_error)) {
//                 dbus_set_g_error (error, &dbus_error);
                dbus_error_free (&dbus_error);
                qWarning() << "Failed to initialize ConsoleKit";
                m_hasError  = true;
                return;
        }
//
//         if (!dbus_connection_add_filter (m_systemBus,
//                                          filter,
//                                          this,
//                                          NULL)) {
// //                 *error = g_error_new_literal (POLKIT_GNOME_CONTEXT_ERROR,
// //                                               POLKIT_GNOME_CONTEXT_ERROR_FAILED,
// //                                               "Cannot add D-Bus filter");
// //                 goto error;
// qWarning() << "Failed to dbus_connection_add_filter";
//                 m_hasError  = true;
//                 return;
//         }

//     DBusConnection *con;

//     con = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    pkTracker = polkit_tracker_new();
    polkit_tracker_set_system_bus_connection(pkTracker, m_systemBus);

    if (dbus_error_is_set (&dbus_error)) {
        m_hasError  = true;
        m_lastError = QString("DBus error name: %1. message: %2").arg(dbus_error.name).arg(dbus_error.message);
        if (pkContext != NULL) {
            polkit_context_unref(pkContext);
        }
        if (pkTracker != NULL) {
            polkit_tracker_unref(pkTracker);
        }

        dbus_error_free (&dbus_error);
        return;
    }
    polkit_tracker_init (pkTracker);
    m_lastError.clear();
    m_hasError = false;
}

void Context::dbusFilter(const QDBusMessage &message)
{
    qDebug() << "=============================================" << message.service();

qDebug() << message.path();
qDebug() << message.interface() << DBUS_INTERFACE_DBUS;
qDebug() << message.member();

    // forward only NameOwnerChanged and ConsoleKit signals to PolkitTracker
    if ((message.type()        == QDBusMessage::SignalMessage
        && message.interface() == "org.freedesktop.DBus"
        && message.member()    == "NameOwnerChanged")
        || (!message.interface().isEmpty()
        && message.interface().startsWith("org.freedesktop.ConsoleKit")) ) {
        qDebug() << "inside";
        DBusMessage *msg = 0;
        msg = dbus_message_new_method_call(message.service().toUtf8().data(),
                message.path().toUtf8().data(), message.interface().toUtf8().data(),
                message.member().toUtf8().data());
        if (msg && polkit_tracker_dbus_func(pkTracker, msg)) {
            qDebug() << "++++++++++++++++++++++ EMIT CONSOLE_KIT_DB_CHANGED";
            emit consoleKitDBChanged();
        }
    }
    qDebug() << "====================END======================" << message.service();
}

bool Context::hasError()
{
    if (m_hasError) {
        // try init again maybe something get
        // back to normal (as DBus might restarted, crashed...)
        init();
    }
    return m_hasError;
}

QString Context::lastError() const
{
    return m_lastError;
}

int Context::io_add_watch(PolKitContext *context, int fd)
{
    qDebug() << "add_watch" << context << fd;

    QSocketNotifier *notify = new QSocketNotifier(fd, QSocketNotifier::Read, m_self);
    m_self->m_watches[fd] = notify;
    notify->connect(notify, SIGNAL(activated(int)), m_self, SLOT(watchActivatedContext(int)));

    return fd; // use simply the fd as the unique id for the watch
}

void Context::watchActivatedContext(int fd)
{
    Q_ASSERT(m_watches.contains(fd));

//    kDebug() << "watchActivated" << fd;

    polkit_context_io_func(pkContext, fd);
}

void Context::io_remove_watch(PolKitContext *context, int id)
{
    Q_ASSERT(id > 0);
    qDebug() << "remove_watch" << context << id;
    if (!m_self->m_watches.contains(id))
        return; // policykit likes to do this more than once

    QSocketNotifier *notify = m_self->m_watches.take(id);
    notify->deleteLater();
    notify->setEnabled(false);
}

void Context::pk_config_changed(PolKitContext *context, void *user_data)
{
    Q_UNUSED(context);
    Q_UNUSED(user_data);
    qDebug() << "PolicyKit reports that the config have changed";
    emit m_self->configChanged();
}

#include "polkit_qt_context.moc"
