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

#include <QtCore>
#include <QSocketNotifier>

#include <polkit-dbus/polkit-dbus.h>

using namespace PolKitQt;

PkContext* PkContext::m_self = 0;

PkContext* PkContext::instance()
{
    if(!m_self)
        new PkContext(qApp);

    return m_self;
}

// I'm using null instead of 0 as polkit will return
// NULL on failures
PkContext::PkContext(QObject *parent)
 : QObject(parent), pkContext(NULL),
   pkTracker(NULL), m_hasError(false)
{
    Q_ASSERT(!m_self);
    m_self = this;

    qDebug() << "PkContext - Constructing singleton";
    init();
}

PkContext::~PkContext()
{
    if (pkContext != NULL) {
        polkit_context_unref(pkContext);
    }
    if (pkTracker != NULL) {
        polkit_tracker_unref(pkTracker);
    }
}

void PkContext::init()
{
//         DBusError dbus_error;
        PolKitError *pk_error;

//         if ((_singleton->priv->system_bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, error)) == NULL) {
//                 goto error;
//         }

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

//         dbus_error_init (&dbus_error);
//
//         /* need to listen to NameOwnerChanged */
//         dbus_bus_add_match (dbus_g_connection_get_connection (_singleton->priv->system_bus),
//                             "type='signal'"
//                             ",interface='"DBUS_INTERFACE_DBUS"'"
//                             ",sender='"DBUS_SERVICE_DBUS"'"
//                             ",member='NameOwnerChanged'",
//                             &dbus_error);
//
//         if (dbus_error_is_set (&dbus_error)) {
//                 dbus_set_g_error (error, &dbus_error);
//                 dbus_error_free (&dbus_error);
//                 goto error;
//         }
//
//         /* need to listen to ConsoleKit signals */
//         dbus_bus_add_match (dbus_g_connection_get_connection (_singleton->priv->system_bus),
//                             "type='signal',sender='org.freedesktop.ConsoleKit'",
//                             &dbus_error);
//
//         if (dbus_error_is_set (&dbus_error)) {
//                 dbus_set_g_error (error, &dbus_error);
//                 dbus_error_free (&dbus_error);
//                 goto error;
//         }
//
//         if (!dbus_connection_add_filter (dbus_g_connection_get_connection (_singleton->priv->system_bus),
//                                          _filter,
//                                          _singleton,
//                                          NULL)) {
//                 *error = g_error_new_literal (POLKIT_GNOME_CONTEXT_ERROR,
//                                               POLKIT_GNOME_CONTEXT_ERROR_FAILED,
//                                               "Cannot add D-Bus filter");
//                 goto error;
//         }

    DBusError dbus_error;
    DBusConnection *con;

    dbus_error_init(&dbus_error);
    con = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    pkTracker = polkit_tracker_new();
    polkit_tracker_set_system_bus_connection(pkTracker, con);

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

bool PkContext::hasError()
{
    if (m_hasError) {
        // try init again maybe something get
        // back to normal (as DBus might restarted, crashed...)
        init();
    }
    return m_hasError;
}

QString PkContext::lastError() const
{
    return m_lastError;
}

int PkContext::io_add_watch(PolKitContext *context, int fd)
{
    qDebug() << "add_watch" << context << fd;

    QSocketNotifier *notify = new QSocketNotifier(fd, QSocketNotifier::Read, m_self);
    m_self->m_watches[fd] = notify;
    notify->connect(notify, SIGNAL(activated(int)), m_self, SLOT(watchActivatedContext(int)));

    return fd; // use simply the fd as the unique id for the watch
}

void PkContext::watchActivatedContext(int fd)
{
    Q_ASSERT(m_watches.contains(fd));

//    kDebug() << "watchActivated" << fd;

    polkit_context_io_func(pkContext, fd);
}

void PkContext::io_remove_watch(PolKitContext *context, int id)
{
    Q_ASSERT(id > 0);
    qDebug() << "remove_watch" << context << id;
    if (!m_self->m_watches.contains(id))
        return; // policykit likes to do this more than once

    QSocketNotifier *notify = m_self->m_watches.take(id);
    notify->deleteLater();
    notify->setEnabled(false);
}

void PkContext::pk_config_changed(PolKitContext *context, void *user_data)
{
    Q_UNUSED(context);
    Q_UNUSED(user_data);
    qDebug() << "PolicyKit reports that the config have changed";
    emit m_self->configChanged();
}

#include "polkit_qt_context.moc"
