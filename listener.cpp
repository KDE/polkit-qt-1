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

#include <QtCore/QDebug>

#include "listener.h"
#include "authority.h"

using namespace PolkitQtAgent;

Listener::Listener(QObject *parent)
        : QObject(parent)
{
    m_listener = polkit_qt_listener_new ();
    
    qDebug() << "New PolkitAgentListener " << m_listener;
    
    ListenerAdapter::instance()->addListener(this);
}

Listener::~Listener()
{
    qDebug("Destroying listener");
    
    ListenerAdapter::instance()->removeListener(this);
}

bool Listener::registerListener(const QString &objectPath)
{
    GError *error;

    PolkitSubject *session = NULL;

    session = polkit_unix_session_new_for_process_sync (getpid (), NULL, &error);
    
    polkit_agent_register_listener (m_listener,
				    session,
                                       objectPath.toAscii().data(),
                                       &error);
    
}

const PolkitAgentListener * Listener::listener()
{
    return m_listener;
}
