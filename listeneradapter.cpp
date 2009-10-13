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

#include "listeneradapter.h"
#include <QtCore/QDebug>

using namespace PolkitQtAgent;

class ListenerAdapterHelper
{
public:
    ListenerAdapterHelper() : q(0) {}
    ~ListenerAdapterHelper() {
        delete q;
    }
    ListenerAdapter *q;
};

AsyncResult::AsyncResult(GSimpleAsyncResult *result) : m_result(result)
{
}

void AsyncResult::complete()
{
    g_simple_async_result_complete(m_result);
}

void AsyncResult::setError(int code, QString text)
{
    g_simple_async_result_set_error(m_result, G_IO_ERROR, code, text.toUtf8().data());
}



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
    m_finishedSignals = new QSignalMapper(this);
    connect(m_finishedSignals, SIGNAL(mapped(QObject *)), this, SLOT(authenticationFinished(QObject *)));
}

Listener* ListenerAdapter::findListener(PolkitAgentListener *listener)
{
    Listener *listItem;

    foreach (listItem, m_listeners)
    {
        Q_ASSERT(listItem);

        if (listItem->listener() == listener)
            return listItem;

    }
    
    return NULL;
}

void ListenerAdapter::polkit_qt_listener_initiate_authentication (PolkitAgentListener  *listener,
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
    
    QList<PolkitQt::Identity *> idents;
    PolkitQt::Details *dets;
    
    Listener *list = findListener(listener);
    
    for (GList *identity = g_list_first(identities); identity != NULL; identity = g_list_next(identity))
        idents.append(new PolkitQt::Identity((PolkitIdentity *)identity->data));

    dets = new PolkitQt::Details(details);

    // FIXME: what if more initializations occures at once
    AsyncResult *res = new AsyncResult(result);
    connect(list, SIGNAL(finished()), m_finishedSignals, SLOT(map()));
    m_finishedSignals->setMapping(list, res);

    list->initiateAuthentication(action_id, message, icon_name, dets, cookie, idents);
}

void ListenerAdapter::authenticationFinished(QObject *result)
{
    qDebug() << "authenticationFinished";
    ((AsyncResult *) (result))->complete();
}

gboolean ListenerAdapter::polkit_qt_listener_initiate_authentication_finish (PolkitAgentListener  *listener,
                                                                             GAsyncResult         *res,
                                                                             GError              **error)
{
    qDebug() << "polkit_qt_listener_initiate_authentication_finish callback for " << listener;

    Listener *list = findListener(listener);

    return list->initiateAuthenticationFinish();
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

