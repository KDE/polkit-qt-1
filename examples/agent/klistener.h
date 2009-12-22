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

#ifndef POLKIT_QT_AGENT_KDE_LISTENER_H
#define POLKIT_QT_AGENT_KDE_LISTENER_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "agent/polkitqt1-agent-listener.h"
#include "core/polkitqt1-identity.h"
#include "core/polkitqt1-details.h"
#include "agent/polkitqt1-agent-session.h"

class KListener : public PolkitQt1::Agent::Listener
{
    Q_OBJECT
    Q_DISABLE_COPY(KListener)
public:
    KListener(QObject *parent = 0);
    ~KListener() {};
public slots:
    void initiateAuthentication(const QString &actionId,
                                const QString &message,
                                const QString &iconName,
                                PolkitQt1::Details *details,
                                const QString &cookie,
                                QList<PolkitQt1::Identity *> identities,
                                PolkitQt1::Agent::AsyncResult *result);
    bool initiateAuthenticationFinish();
    void cancelAuthentication();

    void request(const QString &request, bool echo);
    void completed(bool gainedAuthorization);
    void showError(const QString &text);
    void showInfo(const QString &text);
};

#endif
