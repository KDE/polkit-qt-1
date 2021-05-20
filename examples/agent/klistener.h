/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKIT_QT_AGENT_KDE_LISTENER_H
#define POLKIT_QT_AGENT_KDE_LISTENER_H

#include <QObject>
#include <QString>

#include <polkitqt1-agent-listener.h>
#include <polkitqt1-identity.h>
#include <polkitqt1-details.h>
#include <polkitqt1-agent-session.h>

class KListener : public PolkitQt1::Agent::Listener
{
    Q_OBJECT
    Q_DISABLE_COPY(KListener)
public:
    KListener(QObject *parent = nullptr);
    ~KListener() override {};
public Q_SLOTS:
    void initiateAuthentication(const QString &actionId,
                                const QString &message,
                                const QString &iconName,
                                const PolkitQt1::Details &details,
                                const QString &cookie,
                                const PolkitQt1::Identity::List &identities,
                                PolkitQt1::Agent::AsyncResult *result) override;
    bool initiateAuthenticationFinish() override;
    void cancelAuthentication() override;

    void request(const QString &request, bool echo);
    void completed(bool gainedAuthorization);
    void showError(const QString &text);
    void showInfo(const QString &text);
};

#endif
