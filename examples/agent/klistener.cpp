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
#include <QInputDialog>

#include "klistener.h"
#include "agent/session.h"

using namespace PolkitQtAgent;

KListener::KListener(QObject *parent)
        : Listener(parent)
{
    qDebug() << "Registering KDE listener";
}

// README: this is just testing code...

void KListener::initiateAuthentication(const QString &actionId,
                                       const QString &message,
                                       const QString &iconName,
                                       PolkitQt::Details *details,
                                       const QString &cookie,
                                       QList<PolkitQt::Identity *> identities,
                                       AsyncResult *result)
{
    qDebug() << "initiateAuthentication for " << actionId << " with message " << message;
    qDebug() << "iconName " << iconName;
    qDebug() << details->getKeys();
    qDebug() << "cookie" << cookie;
    
    PolkitQt::Identity *identity;
    
    foreach(identity, identities)
    {
	qDebug() << identity->toString();
	Session *session;
	session = new Session(identity, cookie, result);
	connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
	connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
	connect(session, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
	connect(session, SIGNAL(showInfo(QString)), this, SLOT(showInfo(QString)));	
	session->initiate();
    }
    
    
}

bool KListener::initiateAuthenticationFinish()
{
    qDebug() << "initiateAuthenticationFinish()";
    return true;
}

void KListener::cancelAuthentication()
{
    qDebug() << "Cancelling authentication";
}
    
void KListener::request(const QString &request, bool echo)
{
    qDebug() << "Request: " << request;
    
    Session *session = (Session *)sender();
    
    session->response("");
}

void KListener::completed(bool gainedAuthorization)
{
    qDebug() << "Completed: " << gainedAuthorization;
    Session *session = (Session *)sender();

    session->result()->complete();
    
    delete session;
}

void KListener::showError(const QString &text)
{
    qDebug() << "Error: " << text;
}

void KListener::showInfo(const QString &text)
{
    qDebug() << "Info: " << text;
}
