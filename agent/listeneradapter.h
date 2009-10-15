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

#ifndef POLKIT_QT_AGENT_LISTENER_ADAPTER_H
#define POLKIT_QT_AGENT_LISTENER_ADAPTER_H

#include <QtCore/QObject>

#include "listener.h"
#include <PolkitQt/Identity>
#include <PolkitQt/Details>

typedef struct _GList GList;
typedef struct _GCancellable GCancellable;
typedef struct _GAsyncResult GAsyncResult;
typedef struct _GSimpleAsyncResult GSimpleAsyncResult;
typedef struct _GError GError;
typedef int gboolean;
typedef char gchar;

/**
 * \namespace PolkitQtAgent PolkitQtAgent
 *
 * \brief Namespace wrapping Polkit-Qt Agent classes
 *
 * This namespace wraps all Polkit-Qt Agent classes.
 */
namespace PolkitQtAgent
{
class AsyncResult;
class Listener;
class ListenerAdapter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ListenerAdapter)
public:
    static ListenerAdapter* instance();
    ~ListenerAdapter() {}
    
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
    
    explicit ListenerAdapter(QObject *parent = 0); 
    QList<Listener *> m_listeners;
    
    friend class Listener;
};

}

#endif
