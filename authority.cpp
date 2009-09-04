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

class Authority::Private
{
public:
    // Polkit will return NULL on failures, hence we use it instead of 0
    Private(Authority *qq) : q(qq)
            , pkAuthority(NULL)
            , m_hasError(false) {}

    void init();
    void dbusFilter(const QDBusMessage &message);

    Authority *q;
    PolkitAuthority *pkAuthority;
    bool m_hasError;
    QString m_lastError;
    QDBusConnection *m_systemBus;

    static void pk_config_changed();

    QDomDocument introspect(const QString &service, const QString &path, const QDBusConnection &c);
    QStringList getSignals(const QDomDocument &iface);
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
    

    /*
    // need to listen to NameOwnerChanged
    if (QDBusConnection::systemBus().connect(DBUS_SERVICE_DBUS, QString(), DBUS_INTERFACE_DBUS, "NameOwnerChanged",
            Authority::instance(), SLOT(dbusFilter(const QDBusMessage &)))) {
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
                Authority::instance(),
                SLOT(dbusFilter(const QDBusMessage &)))) {
            qWarning() << "Could not connect to the service bus to listen "
            "to the following signals:"
            << sigs;
        }

    }
*/
    /*if (dbus_error_is_set(&dbus_error)) {
        dbus_error_free(&dbus_error);
        qWarning() << "Failed to initialize ConsoleKit";
        m_hasError  = true;
        return;
    }*/

    m_lastError.clear();
    m_hasError = false;
}

void Authority::Private::dbusFilter(const QDBusMessage &message)
{
    // forward only NameOwnerChanged and ConsoleKit signals to PolkitTracker
    /*if ((message.type() == QDBusMessage::SignalMessage &&
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

        if (msg) {
            emit q->consoleKitDBChanged();
        }

    }*/
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

QDomDocument Authority::Private::introspect(const QString &service, const QString &path, const QDBusConnection &c)
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

QStringList Authority::Private::getSignals(const QDomDocument &doc)
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

PolkitAuthority *Authority::getPolkitAuthority() const
{
    return d->pkAuthority;
}

#include "moc_authority.cpp"
