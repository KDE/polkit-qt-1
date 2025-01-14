/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "listeneradapter_p.h"
#include <QDebug>
#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1
#include <polkitagent/polkitagent.h>

namespace PolkitQt1
{

namespace Agent
{

class ListenerAdapterHelper
{
public:
    ListenerAdapterHelper() : q(nullptr) {}
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

    return nullptr;
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

    PolkitQt1::Details dets(details);

    Listener *list = findListener(listener);

    // Polkit enumerates identities without regard for their hash value, potentially leading to duplicated entries.
    // Unique the identities on our end.
    // https://github.com/polkit-org/polkit/issues/542
    QHash<guint, PolkitQt1::Identity> uniqueIdentities;
    for (GList *entry = g_list_first(identities); entry != nullptr; entry = g_list_next(entry)) {
        auto identity = static_cast<PolkitIdentity *>(entry->data);
        uniqueIdentities.insert(polkit_identity_hash(identity), PolkitQt1::Identity(identity));
    }

    list->initiateAuthentication(QString::fromUtf8(action_id),
                                 QString::fromUtf8(message),
                                 QString::fromUtf8(icon_name),
                                 dets,
                                 QString::fromUtf8(cookie),
                                 uniqueIdentities.values(),
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
