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

#ifndef POLKIT_QT_AGENT_LISTENER_H
#define POLKIT_QT_AGENT_LISTENER_H

#include <QtCore/QObject>

#include <PolkitQtAgent/Session>

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

typedef struct _PolkitAgentListener PolkitAgentListener;

namespace PolkitQt {
    class Subject;
    class Identity;
    class Details;
}

/**
 * \namespace PolkitQtAgent PolkitQtAgent
 *
 * \brief Namespace wrapping Polkit-Qt Agent classes
 *
 * This namespace wraps all Polkit-Qt Agent classes.
 */
namespace PolkitQtAgent
{

class ListenerPrivate;
class Listener : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Listener)
public:
    Listener(QObject *parent = 0);
    explicit Listener(PolkitAgentListener *listener, QObject *parent = 0);
    virtual ~Listener();

    bool registerListener(PolkitQt::Subject *subject, const QString &objectPath);
    const PolkitAgentListener *listener();

public Q_SLOTS:
    virtual void initiateAuthentication(const QString &actionId,
                                        const QString &message,
                                        const QString &iconName,
                                        PolkitQt::Details *details,
                                        const QString &cookie,
                                        QList<PolkitQt::Identity *> identities,
                                        AsyncResult *result) = 0;
    virtual bool initiateAuthenticationFinish() = 0;
    virtual void cancelAuthentication() = 0;

Q_SIGNALS:
    void finished();

private:
    ListenerPrivate * const d;
};
}

#endif
