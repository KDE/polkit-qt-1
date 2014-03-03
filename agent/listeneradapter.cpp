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

#include "listeneradapter_p.h"
#include <QtCore/QDebug>
#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1
#include <polkitagent/polkitagent.h>

namespace PolkitQt1
{

namespace Agent
{

class ListenerAdapterHelper
{
public:
    ListenerAdapterHelper() : q(0) {}
    ~ListenerAdapterHelper() {
        delete q;
    }
    ListenerAdapter *q;
};

Q_GLOBAL_STATIC(ListenerAdapterHelper, s_globalListenerAdapter)

ListenerAdapter *ListenerAdapter::instance()
{
    if (!s_globalListenerAdapter()->q) {
        new ListenerAdapter();
    }

    return s_globalListenerAdapter()->q;
}

ListenerAdapter::ListenerAdapter(QObject *parent)
        : QObject(parent)
{
    Q_ASSERT(!s_globalListenerAdapter()->q);
    s_globalListenerAdapter()->q = this;
}

Listener* ListenerAdapter::findListener(PolkitAgentListener *listener)
{
    Listener *listItem;

    Q_FOREACH(listItem, m_listeners) {
        Q_ASSERT(listItem);

        if (listItem->listener() == listener) {
            return listItem;
        }
    }

    return NULL;
}

void ListenerAdapter::polkit_qt_listener_initiate_authentication(PolkitAgentListener  *listener,
        const gchar          *action_id,
        const gchar          *message,
        const gchar          *icon_name,
        PolkitDetails        *details,
        const gchar          *cookie,
        GList                *identities,
        GCancellable         *cancellable,
        GSimpleAsyncResult *result)
{
    qDebug() << "polkit_qt_listener_initiate_authentication callback for " << listener;

    PolkitQt1::Identity::List idents;
    PolkitQt1::Details dets(details);

    Listener *list = findListener(listener);

    for (GList *identity = g_list_first(identities); identity != NULL; identity = g_list_next(identity)) {
         idents.append(PolkitQt1::Identity((PolkitIdentity *)identity->data));
    }

    list->initiateAuthentication(QString::fromUtf8(action_id),
                                 QString::fromUtf8(message),
                                 QString::fromUtf8(icon_name),
                                 dets,
                                 QString::fromUtf8(cookie),
                                 idents,
                                 new AsyncResult(result));
}

gboolean ListenerAdapter::polkit_qt_listener_initiate_authentication_finish(PolkitAgentListener  *listener,
        GAsyncResult         *res,
        GError              **error)
{
    qDebug() << "polkit_qt_listener_initiate_authentication_finish callback for " << listener;

    GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT(res);
    if (g_simple_async_result_propagate_error(simple, error)) {
        return false;
    }
    return true;
}

void ListenerAdapter::cancelled_cb(PolkitAgentListener *listener)
{
    qDebug() << "cancelled_cb for " << listener;

    Listener *list = findListener(listener);

    list->cancelAuthentication();
}

void ListenerAdapter::addListener(Listener *listener)
{
    qDebug() << "Adding new listener " << listener << "for " << listener->listener();

    m_listeners.append(listener);
}

void ListenerAdapter::removeListener(Listener *listener)
{
    qDebug() << "Removing listener " << listener;

    // should be safe as we don't have more than one same listener registered in one time
    m_listeners.removeOne(listener);
}

}

}
