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
 *
 * polkit-qt-listener based on code by David Zeuthen <davidz@redhat.com>
 */


#include "polkitqtlistener.h"
#include <stdio.h>

#include <QtCore/QDebug>

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

using namespace PolkitQtAgent;

/**
  * \internal
  */
struct _PolkitQtListener
{
    PolkitAgentListener parent_instance;
};

/**
  * \internal
  */
struct _PolkitQtListenerClass
{
    PolkitAgentListenerClass parent_class;
};

static void polkit_qt_listener_initiate_authentication (PolkitAgentListener  *listener,
                                                        const gchar          *action_id,
                                                        const gchar          *message,
                                                        const gchar          *icon_name,
                                                        PolkitDetails        *details,
                                                        const gchar          *cookie,
                                                        GList                *identities,
                                                        GCancellable         *cancellable,
                                                        GAsyncReadyCallback   callback,
                                                        gpointer              user_data);

static gboolean polkit_qt_listener_initiate_authentication_finish (PolkitAgentListener  *listener,
                                                                   GAsyncResult         *res,
                                                                   GError              **error);

G_DEFINE_TYPE (PolkitQtListener, polkit_qt_listener, POLKIT_AGENT_TYPE_LISTENER);

static void polkit_qt_listener_init (PolkitQtListener *listener)
{
  g_type_init();
}

static void polkit_qt_listener_finalize (GObject *object)
{
    PolkitQtListener *listener;

    listener = POLKIT_QT_LISTENER (object);

    if (G_OBJECT_CLASS (polkit_qt_listener_parent_class)->finalize != NULL)
        G_OBJECT_CLASS (polkit_qt_listener_parent_class)->finalize (object);
}

static void polkit_qt_listener_class_init (PolkitQtListenerClass *klass)
{
    GObjectClass *gobject_class;
    PolkitAgentListenerClass *listener_class;

    gobject_class = G_OBJECT_CLASS (klass);
    listener_class = POLKIT_AGENT_LISTENER_CLASS (klass);

    gobject_class->finalize = polkit_qt_listener_finalize;

    listener_class->initiate_authentication          = polkit_qt_listener_initiate_authentication;
    listener_class->initiate_authentication_finish   = polkit_qt_listener_initiate_authentication_finish;
}

PolkitAgentListener *polkit_qt_listener_new (void)
{
    return POLKIT_AGENT_LISTENER (g_object_new (POLKIT_QT_TYPE_LISTENER, NULL));
}

static void cancelled_cb (GCancellable *cancellable, gpointer user_data)
{
    ListenerAdapter::instance()->cancelled_cb((PolkitAgentListener *)user_data);
}

static void polkit_qt_listener_initiate_authentication (PolkitAgentListener  *agent_listener,
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
    qDebug() << "Listener adapter polkit_qt_listener_initiate_authentication";
    PolkitQtListener *listener = POLKIT_QT_LISTENER (agent_listener);

    // The result of asynchronous method will be created here and it will be pushed to the listener.
    GSimpleAsyncResult *result = g_simple_async_result_new((GObject *) listener, callback, user_data, agent_listener);
    qDebug() << "GSimpleAsyncResult:" << result;

    ListenerAdapter::instance()->polkit_qt_listener_initiate_authentication(agent_listener,
                                                                            action_id,
                                                                            message,
                                                                            icon_name,
                                                                            details,
                                                                            cookie,
                                                                            identities,
                                                                            cancellable,
                                                                            result);

    if (cancellable != NULL)
    {
        g_signal_connect (cancellable,
                          "cancelled",
                          G_CALLBACK (cancelled_cb),
                          agent_listener);
    }

}

static gboolean polkit_qt_listener_initiate_authentication_finish (PolkitAgentListener  *listener,
                                                                   GAsyncResult         *res,
                                                                   GError              **error)
{
    qDebug() << "Listener adapter polkit_qt_listener_initiate_authentication_finish";  
    return ListenerAdapter::instance()->polkit_qt_listener_initiate_authentication_finish(listener,
                                                                                          res,
                                                                                          error);
}

