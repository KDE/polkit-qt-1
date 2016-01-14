/*
 * This file is part of the Polkit-qt project
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

#include "polkitqt1-agent-listener.h"

#include <QtCore/QDebug>

#include "polkitqtlistener_p.h"

#include "polkitqt1-authority.h"
#include "polkitqt1-subject.h"
#include "polkitqt1-identity.h"
#include "polkitqt1-details.h"

namespace PolkitQt1
{

namespace Agent
{

class ListenerPrivate
{
public:
    PolkitAgentListener *listener;
#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
    void *registeredHandle;
#endif
};

Listener::Listener(QObject *parent)
        : QObject(parent), d(new ListenerPrivate)
{
    g_type_init();

    d->listener = polkit_qt_listener_new();

    qDebug() << "New PolkitAgentListener " << d->listener;

    ListenerAdapter::instance()->addListener(this);
}

Listener::Listener(PolkitAgentListener *listener, QObject *parent)
        : QObject(parent), d(new ListenerPrivate)
{
    g_type_init();

    d->listener = listener;
    
    if (d->listener != NULL) {
        g_object_ref(d->listener);
    }
}

Listener::~Listener()
{
    qDebug("Destroying listener");

    ListenerAdapter::instance()->removeListener(this);
#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
    if (d->registeredHandle) {
        polkit_agent_listener_unregister(d->registeredHandle);
    }
#endif
    if (d->listener != NULL) {
        g_object_unref(d->listener);
    }
}

bool Listener::registerListener(const PolkitQt1::Subject &subject, const QString &objectPath)
{
    GError *error = NULL;

#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
    d->registeredHandle = polkit_agent_listener_register(d->listener,
                                            POLKIT_AGENT_REGISTER_FLAGS_NONE,
#else
    bool r = polkit_agent_register_listener(d->listener,
#endif
                                            subject.subject(),
                                            objectPath.toLatin1().data(),
#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
                                            NULL,
#endif
                                            &error);

    if (error != NULL) {
        qWarning() << QString("Cannot register authentication agent: %1").arg(error->message);
        g_error_free(error);
        return false;
    }
#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
    if (d->registeredHandle == NULL) {
        qWarning() << QString("Cannot register authentication agent!");
        return false;
    }
    return true;
#else
    return r;
#endif
}

const PolkitAgentListener *Listener::listener()
{
    return d->listener;
}

}

}
