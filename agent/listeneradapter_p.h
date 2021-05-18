/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LISTENER_ADAPTER_P_H
#define LISTENER_ADAPTER_P_H

#include <QtCore/QObject>

#include "polkitqt1-agent-listener.h"
#include "polkitqt1-identity.h"
#include "polkitqt1-details.h"

typedef struct _GList GList;
typedef struct _GCancellable GCancellable;
typedef struct _GAsyncResult GAsyncResult;
typedef struct _GSimpleAsyncResult GSimpleAsyncResult;
typedef struct _GError GError;
typedef int gboolean;
typedef char gchar;

namespace PolkitQt1
{

namespace Agent
{

class AsyncResult;
class Listener;
class ListenerAdapter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ListenerAdapter)
public:
    static ListenerAdapter* instance();
    ~ListenerAdapter() override {}

    void polkit_qt_listener_initiate_authentication(PolkitAgentListener  *listener,
            const gchar          *action_id,
            const gchar          *message,
            const gchar          *icon_name,
            PolkitDetails        *details,
            const gchar          *cookie,
            GList                *identities,
            GCancellable         *cancellable,
            GSimpleAsyncResult   *result);

    gboolean polkit_qt_listener_initiate_authentication_finish(PolkitAgentListener  *listener,
            GAsyncResult         *res,
            GError               **error);
    void cancelled_cb(PolkitAgentListener *listener);
private:
    void addListener(Listener *listener);
    void removeListener(Listener *listener);
    Listener* findListener(PolkitAgentListener *listener);

    explicit ListenerAdapter(QObject *parent = nullptr);
    QList<Listener *> m_listeners;

    friend class Listener;
};

}

}

#endif
