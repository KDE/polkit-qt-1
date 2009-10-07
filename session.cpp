/*
 * This file is part of the PolKit1-qt project
 * Copyright (C) 2009 Radek Novacek <rnovacek@redhat.com>
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
#include <QtGui/QInputDialog>

#include "session.h"
#include "identity.h"

using namespace PolkitQtAgent;

Session::Session(PolkitQt::Identity *identity, const QString &cookie, QObject *parent) : QObject(parent)
{
    m_polkitAgentSession = polkit_agent_session_new(identity->identity(), cookie.toUtf8().data());
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "completed", G_CALLBACK(_completed), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "request", G_CALLBACK(_request), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "show-error", G_CALLBACK(_showError), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "show-info", G_CALLBACK(_showInfo), this);
}

Session::Session(PolkitAgentSession *pkAgentSession, QObject *parent) : m_polkitAgentSession(pkAgentSession), QObject(parent)
{
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "completed", G_CALLBACK(_completed), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "request", G_CALLBACK(_request), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "show-error", G_CALLBACK(_showError), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "show-info", G_CALLBACK(_showInfo), this);
}

Session::~Session()
{
    g_object_unref(m_polkitAgentSession);
}

void Session::initiate()
{
    polkit_agent_session_initiate(m_polkitAgentSession);
}

void Session::response(const QString &response)
{
    polkit_agent_session_response(m_polkitAgentSession, response.toUtf8().data());
}

void Session::cancel()
{
    polkit_agent_session_cancel(m_polkitAgentSession);
}

void Session::_completed(PolkitAgentSession *s, gboolean gained_authorization, gpointer user_data)
{
    qDebug() << "COMPLETED";
    emit ((Session *)user_data)->completed(gained_authorization);
}

void Session::_request(PolkitAgentSession *s, gchar *request, gboolean echo_on, gpointer user_data)
{
    qDebug() << "REQUEST";
    emit ((Session *)user_data)->request(QString::fromUtf8(request), echo_on);
}

void Session::_showError(PolkitAgentSession *s, gchar *text, gpointer user_data)
{
    qDebug() << "showError";
    emit ((Session *)user_data)->showError(QString::fromUtf8(text));
}

void Session::_showInfo(PolkitAgentSession *s, gchar *text, gpointer user_data)
{
    qDebug() << "showInfo";
    emit ((Session *)user_data)->showInfo(QString::fromUtf8(text));
}
