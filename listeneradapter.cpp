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

void ListenerAdapter::polkit_qt_listener_initiate_authentication (PolkitAgentListener  *listener,
                                                           const gchar          *action_id,
                                                           const gchar          *message,
                                                           const gchar          *icon_name,
                                                           PolkitDetails        *details,
                                                           const gchar          *cookie,
                                                           GList                *identities,
                                                           GCancellable         *cancellable,
                                                           GAsyncReadyCallback   callback,
                                                           gpointer              user_data)
{
    qDebug() << "polkit_qt_listener_initiate_authentication callback for " << listener;
    
    QList<PolkitQt::Identity *> idents;
    
    Listener *listItem;
    
    foreach (listItem, m_listeners)
    {
	//Q_ASSERT(!listItem);
	if (listItem && (listItem->listener() == listener))
	{
	    qDebug() << "Listener has been found.";
	        
	    GList *glist2;
	    for (glist2 = identities; glist2 != NULL; glist2 = g_list_next(glist2))
	    {
		// FIXME: conversion
		idents.append(PolkitQt::Identity::fromString(
		    QString::fromUtf8(polkit_identity_to_string((PolkitIdentity *) glist2->data))));
		    
		g_object_unref(glist2->data);
	    }

	    g_list_free(identities);

	    listItem->initiateAuthentication(action_id, message, icon_name, cookie, idents);
	}    
    }
}

gboolean ListenerAdapter::polkit_qt_listener_initiate_authentication_finish (PolkitAgentListener  *listener,
                                                                      GAsyncResult         *res,
                                                                      GError              **error)
{
    qDebug() << "polkit_qt_listener_initiate_authentication_finish callback for " << listener;
}

void ListenerAdapter::cancelled_cb(PolkitAgentListener *listener)
{
    qDebug() << "cancelled_cb for " << listener;
    
    // TODO: call cancelAuthentication method 
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
