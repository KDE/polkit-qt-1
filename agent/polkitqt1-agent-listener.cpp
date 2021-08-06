/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polkitqt1-agent-listener.h"

#include <QDebug>

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
    d->listener = polkit_qt_listener_new();

    qDebug() << "New PolkitAgentListener " << d->listener;

    ListenerAdapter::instance()->addListener(this);
}

Listener::Listener(PolkitAgentListener *listener, QObject *parent)
        : QObject(parent), d(new ListenerPrivate)
{
    d->listener = listener;
    
    if (d->listener != nullptr) {
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
    if (d->listener != nullptr) {
        g_object_unref(d->listener);
    }
}

bool Listener::registerListener(const PolkitQt1::Subject &subject, const QString &objectPath)
{
    GError *error = nullptr;

#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
    d->registeredHandle = polkit_agent_listener_register(d->listener,
                                            POLKIT_AGENT_REGISTER_FLAGS_NONE,
#else
    bool r = polkit_agent_register_listener(d->listener,
#endif
                                            subject.subject(),
                                            objectPath.toLatin1().data(),
#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
                                            nullptr,
#endif
                                            &error);

    if (error != nullptr) {
        qWarning() << QString("Cannot register authentication agent: %1").arg(error->message);
        g_error_free(error);
        return false;
    }
#ifndef POLKIT_QT_1_COMPATIBILITY_MODE
    if (d->registeredHandle == nullptr) {
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
