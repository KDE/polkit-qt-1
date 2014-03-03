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

#include "polkitqt1-agent-session.h"

#include <QtCore/QDebug>

#include "polkitqt1-identity.h"

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1
#include <polkitagent/polkitagent.h>

using namespace PolkitQt1::Agent;

class Session::Private
{
public:
    Private() {}
    ~Private();

    static void completed(PolkitAgentSession *s, gboolean gained_authorization, gpointer user_data);
    static void request(PolkitAgentSession *s, gchar *request, gboolean echo_on, gpointer user_data);
    static void showError(PolkitAgentSession *s, gchar *text, gpointer user_data);
    static void showInfo(PolkitAgentSession *s, gchar *text, gpointer user_data);

    AsyncResult *result;
    PolkitAgentSession *polkitAgentSession;
};

Session::Private::~Private()
{
    // polkitAgentSession is freed in Session d'tor
}

Session::Session(const PolkitQt1::Identity &identity, const QString &cookie, AsyncResult *result, QObject *parent)
        : QObject(parent)
        , d(new Private)
{
    d->result = result;
    d->polkitAgentSession = polkit_agent_session_new(identity.identity(), cookie.toUtf8().data());
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "completed", G_CALLBACK(Private::completed), this);
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "request", G_CALLBACK(Private::request), this);
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "show-error", G_CALLBACK(Private::showError), this);
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "show-info", G_CALLBACK(Private::showInfo), this);
}

Session::Session(PolkitAgentSession *pkAgentSession, QObject *parent)
        : QObject(parent)
        , d(new Private)
{
    d->polkitAgentSession = pkAgentSession;
    if (d->polkitAgentSession) {
        g_object_ref(d->polkitAgentSession);
    }
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "completed", G_CALLBACK(Private::completed), this);
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "request", G_CALLBACK(Private::request), this);
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "show-error", G_CALLBACK(Private::showError), this);
    g_signal_connect(G_OBJECT(d->polkitAgentSession), "show-info", G_CALLBACK(Private::showInfo), this);
}

Session::~Session()
{
    if (d->polkitAgentSession)
        g_object_unref(d->polkitAgentSession);

    delete d;
}

void Session::initiate()
{
    polkit_agent_session_initiate(d->polkitAgentSession);
}

void Session::setResponse(const QString &response)
{
    polkit_agent_session_response(d->polkitAgentSession, response.toUtf8().data());
}

void Session::cancel()
{
    polkit_agent_session_cancel(d->polkitAgentSession);
}

AsyncResult *Session::result()
{
    return d->result;
}

void Session::Private::completed(PolkitAgentSession *s, gboolean gained_authorization, gpointer user_data)
{
    qDebug() << "COMPLETED";
    Session *session = (Session *)user_data;
    Q_EMIT(session)->completed(gained_authorization);

    //free session here as polkit documentation asks
    g_object_unref(session->d->polkitAgentSession);
    session->d->polkitAgentSession = 0;
}

void Session::Private::request(PolkitAgentSession *s, gchar *request, gboolean echo_on, gpointer user_data)
{
    qDebug() << "REQUEST";
    Q_EMIT((Session *)user_data)->request(QString::fromUtf8(request), echo_on);
}

void Session::Private::showError(PolkitAgentSession *s, gchar *text, gpointer user_data)
{
    qDebug() << "showError";
    Q_EMIT((Session *)user_data)->showError(QString::fromUtf8(text));
}

void Session::Private::showInfo(PolkitAgentSession *s, gchar *text, gpointer user_data)
{
    qDebug() << "showInfo";
    Q_EMIT((Session *)user_data)->showInfo(QString::fromUtf8(text));
}

//

class AsyncResult::Private
{
public:
    Private(GSimpleAsyncResult *r) : result(r) {};

    GSimpleAsyncResult *result;
};

AsyncResult::AsyncResult(GSimpleAsyncResult *result)
        : d(new Private(result))
{
}

AsyncResult::~AsyncResult()
{
    if (d->result)
        g_object_unref(d->result);
}

void AsyncResult::setCompleted()
{
    if (d->result == NULL)
        return;
    g_simple_async_result_complete(d->result);
    // Assure that completed won't be called twice
    g_object_unref(d->result);
    d->result = NULL;
}

void AsyncResult::setError(const QString &text)
{
    Q_ASSERT(d->result);
    g_simple_async_result_set_error(d->result, POLKIT_ERROR, POLKIT_ERROR_FAILED, "%s", text.toUtf8().data());
}
