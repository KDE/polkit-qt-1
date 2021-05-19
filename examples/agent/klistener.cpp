/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QDebug>
#include <QInputDialog>

#include "klistener.h"
#include <polkitqt1-agent-session.h>

using namespace PolkitQt1::Agent;

KListener::KListener(QObject *parent)
        : Listener(parent)
{
    qDebug() << "Registering KDE listener";
}

// README: this is just testing code...

void KListener::initiateAuthentication(const QString &actionId,
                                       const QString &message,
                                       const QString &iconName,
                                       const PolkitQt1::Details &details,
                                       const QString &cookie,
                                       const PolkitQt1::Identity::List &identities,
                                       AsyncResult *result)
{
    qDebug() << "initiateAuthentication for " << actionId << " with message " << message;
    qDebug() << "iconName " << iconName;
    qDebug() << details.keys();
    qDebug() << "cookie" << cookie;

    Q_FOREACH (const PolkitQt1::Identity &identity, identities) {
        qDebug() << identity.toString();
        Session *session;
        session = new Session(identity, cookie, result);
        connect(session, SIGNAL(request(QString,bool)), this, SLOT(request(QString,bool)));
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

    session->setResponse("");
}

void KListener::completed(bool gainedAuthorization)
{
    qDebug() << "Completed: " << gainedAuthorization;
    Session *session = (Session *)sender();

    session->result()->setCompleted();

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
