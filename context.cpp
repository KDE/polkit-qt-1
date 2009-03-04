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

#include "context.h"

#include "singleton.h"

#include <QtCore/QSocketNotifier>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtCore/QDebug>

#include <polkit-dbus/polkit-dbus.h>

using namespace PolkitQt;

class ContextHelper
{
public:
    ContextHelper() : q(0) {}
    ~ContextHelper() {
        delete q;
    }
    Context *q;
};

POLKIT_QT_GLOBAL_STATIC(ContextHelper, s_globalContext)

Context *Context::instance()
{
    if (!s_globalContext->q) {
        new Context;
    }

    return s_globalContext->q;
}

class Context::Private
{
public:
    // Polkit will return NULL on failures, hence we use it instead of 0
    Private() : pkContext(NULL)
            , pkTracker(NULL)
            , m_hasError(false) {};

    void init();

    PolKitContext *pkContext;
    PolKitTracker *pkTracker;
    bool m_hasError;
    QString m_lastError;
    DBusConnection *m_systemBus;

    QMap<int, QSocketNotifier*> m_watches;

    static int  io_add_watch(PolKitContext *context, int fd);
    static void io_remove_watch(PolKitContext *context, int fd);
    static void pk_config_changed(PolKitContext *context, void *user_data);

    QDomDocument introspect(const QString &service, const QString &path, const QDBusConnection &c);
    QStringList getSignals(const QDomDocument &iface);
};

Context::Context(QObject *parent)
        : QObject(parent)
        , d(new Private())
{
    Q_ASSERT(!s_globalContext->q);
    s_globalContext->q = this;

    d->init();
}

Context::~Context()
{
    if (d->pkContext != NULL) {
        polkit_context_unref(d->pkContext);
    }
    if (d->pkTracker != NULL) {
        polkit_tracker_unref(d->pkTracker);
    }

    delete d;
}

void Context::Private::init()
{
    DBusError error;
    DBusError dbus_error;
    PolKitError *pk_error;
    dbus_error_init(&error);

    if ((m_systemBus = dbus_bus_get(DBUS_BUS_SYSTEM, &error)) == NULL) {
        qWarning() << "Failed to initialize DBus";
    }

    pkContext = polkit_context_new();
    polkit_context_set_io_watch_functions(pkContext, io_add_watch, io_remove_watch);
    polkit_context_set_config_changed(pkContext, pk_config_changed, Context::instance());

    pk_error = NULL;
    if (!polkit_context_init(pkContext, &pk_error)) {
        qWarning() << "Failed to initialize PolicyKit context: "
                   << polkit_error_get_error_message(pk_error);
        m_lastError = polkit_error_get_error_message(pk_error);
        m_hasError  = true;
        if (pkContext != NULL) {
            polkit_context_unref(pkContext);
        }
        polkit_error_free(pk_error);
        return;
    }

    // TODO FIXME: I'm pretty sure dbus-glib blows in a way that
    // we can't say we're interested in all signals from all
    // members on all interfaces for a given service... So we do
    // this..
    dbus_error_init(&dbus_error);

    // need to listen to NameOwnerChanged
    if (QDBusConnection::systemBus().connect(DBUS_SERVICE_DBUS, QString(), DBUS_INTERFACE_DBUS, "NameOwnerChanged",
            Context::instance(), SLOT(dbusFilter(const QDBusMessage &)))) {
    } else {
        qWarning() << "Could not connect to the service bus to listen to NameOwnerChanged";
    }

    // Ok, let's get what we need here
    QStringList sigs;

    sigs += getSignals(introspect("org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Manager", QDBusConnection::systemBus()));
    sigs += getSignals(introspect("org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Session1", QDBusConnection::systemBus()));
    sigs += getSignals(introspect("org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Seat1", QDBusConnection::systemBus()));

    foreach(const QString &sig, sigs) {

        if (!QDBusConnection::systemBus().connect("org.freedesktop.ConsoleKit",
                                                  QString(),
                                                  QString(),
                                                  sig,
                                                  Context::instance(),
                                                  SLOT(dbusFilter(const QDBusMessage &)))) {
            qWarning() << "Could not connect to the service bus to listen "
                          "to the following signals:"
                       << sigs;
        }

    }

    if (dbus_error_is_set(&dbus_error)) {
        dbus_error_free(&dbus_error);
        qWarning() << "Failed to initialize ConsoleKit";
        m_hasError  = true;
        return;
    }

    pkTracker = polkit_tracker_new();
    polkit_tracker_set_system_bus_connection(pkTracker, m_systemBus);

    if (dbus_error_is_set(&dbus_error)) {
        m_hasError  = true;
        m_lastError = QString("DBus error name: %1. message: %2").arg(dbus_error.name).arg(dbus_error.message);
        if (pkContext != NULL) {
            polkit_context_unref(pkContext);
        }
        if (pkTracker != NULL) {
            polkit_tracker_unref(pkTracker);
        }

        dbus_error_free(&dbus_error);
        return;
    }
    polkit_tracker_init(pkTracker);
    m_lastError.clear();
    m_hasError = false;
}

void Context::dbusFilter(const QDBusMessage &message)
{
    // forward only NameOwnerChanged and ConsoleKit signals to PolkitTracker
    if ((message.type() == QDBusMessage::SignalMessage &&
            message.interface() == "org.freedesktop.DBus" &&
            message.member()    == "NameOwnerChanged")
            ||
            (!message.interface().isEmpty() &&
             message.interface().startsWith(QLatin1String("org.freedesktop.ConsoleKit")))) {

        DBusMessage *msg = 0;
        DBusMessageIter args;
        msg = dbus_message_new_signal(
                  message.path().toUtf8().data(), message.interface().toUtf8().data(),
                  message.member().toUtf8().data());
        dbus_message_iter_init_append(msg, &args);
        foreach(QVariant arg, message.arguments()) {
            char *argument = qstrdup(arg.toString().toAscii());
            int numarg;

            switch (arg.type()) {
            case QVariant::String:
                if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &argument)) {
                    qFatal("Out Of Memory!");
                    exit(1);
                }
                break;
            case QVariant::Bool:
                if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, argument)) {
                    qFatal("Out Of Memory!");
                    exit(1);
                }
                break;
            case QVariant::Int:
                numarg = arg.toInt();
                if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &numarg)) {
                    qFatal("Out Of Memory!");
                    exit(1);
                }
                break;

                // Don't try to understand this part, it's too weird for me too
            case QVariant::UserType:
                if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &argument)) {
                    qFatal("Out Of Memory!");
                    exit(1);
                }
                break;
            default:
                qWarning() << "Type not handled in the switch. It should be a " << arg.type();
                break;
            }


        }

        if (msg && polkit_tracker_dbus_func(d->pkTracker, msg)) {
            emit consoleKitDBChanged();
        }
    }
}

bool Context::hasError()
{
    if (d->m_hasError) {
        // try init again maybe something get
        // back to normal (as DBus might restarted, crashed...)
        d->init();
    }
    return d->m_hasError;
}

QString Context::lastError() const
{
    return d->m_lastError;
}

int Context::Private::io_add_watch(PolKitContext *, int fd)
{
    QSocketNotifier *notify = new QSocketNotifier(fd, QSocketNotifier::Read, Context::instance());
    Context::instance()->d->m_watches[fd] = notify;
    notify->connect(notify, SIGNAL(activated(int)), Context::instance(), SLOT(watchActivatedContext(int)));

    return fd; // use simply the fd as the unique id for the watch
}

void Context::watchActivatedContext(int fd)
{
    Q_ASSERT(d->m_watches.contains(fd));

    polkit_context_io_func(d->pkContext, fd);
}

void Context::Private::io_remove_watch(PolKitContext *, int id)
{
    Q_ASSERT(id > 0);

    if (!Context::instance()->d->m_watches.contains(id)) {
        return; // policykit likes to do this more than once
    }

    QSocketNotifier *notify = Context::instance()->d->m_watches.take(id);
    notify->deleteLater();
    notify->setEnabled(false);
}

void Context::Private::pk_config_changed(PolKitContext *context, void *user_data)
{
    Q_UNUSED(context);
    Q_UNUSED(user_data);
    emit Context::instance()->configChanged();
}

QDomDocument Context::Private::introspect(const QString &service, const QString &path, const QDBusConnection &c)
{
    QDomDocument doc;

    QDBusInterface iface(service, path, "org.freedesktop.DBus.Introspectable", c);
    if (!iface.isValid()) {
        QDBusError err(iface.lastError());
        qWarning() << QString("Cannot introspect object %1 at %2:\n  %3 (%4)\n").arg(path).arg(
            service).arg(err.name()).arg(err.message());
        return doc;
    }

    QDBusReply<QString> xml = iface.call("Introspect");

    if (!xml.isValid()) {
        QDBusError err(xml.error());
        if (err.isValid()) {
            qWarning() << QString("Call to object %1 at %2:\n  %3 (%4) failed\n").arg(
                path).arg(service).arg(err.name()).arg(err.message());
        } else {
            qWarning() << QString("Invalid XML received from object %1 at %2\n").arg(
                path).arg(service);
        }
        return doc;
    }

    doc.setContent(xml);
    return doc;
}

QStringList Context::Private::getSignals(const QDomDocument &doc)
{
    QStringList retlist;

    QDomElement node = doc.documentElement();
    QDomElement child = node.firstChildElement();

    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("node") || child.tagName() == QLatin1String("interface")) {
            QDomElement iface = child.firstChildElement();
            while (!iface.isNull()) {
                if (iface.tagName() == QLatin1String("signal")) {
                    retlist.append(iface.attribute("name"));
                }
                iface = iface.nextSiblingElement();
            }
        }

        child = child.nextSiblingElement();
    }

    return retlist;
}

PolKitContext *Context::getPolKitContext()
{
    return d->pkContext;
}

PolKitTracker *Context::getPolKitTracker()
{
    return d->pkTracker;
}
