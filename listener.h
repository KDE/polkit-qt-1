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
#include <QtCore/QDebug>

#include "listeneradapter.h"
#include "polkitqtlistener.h"
#include "subject.h"
#include "identity.h"

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

/**
 * \namespace PolkitQtAgent PolkitQtAgent
 *
 * \brief Namespace wrapping Polkit-Qt Agent classes
 *
 * This namespace wraps all Polkit-Qt Agent classes.
 */
namespace PolkitQtAgent
{
    
class Listener : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Listener)
public:
    Listener(QObject *parent = 0);
    ~Listener();
    // subject when ported to PolkitQt
    bool registerListener(PolkitQt::Subject *subject, const QString &objectPath);
    const PolkitAgentListener *listener();
public slots:
    virtual void initiateAuthentication(const QString &actionId,
					const QString &message,
					const QString &iconName,
					const QString &cookie,
					QList<PolkitQt::Identity *> identities) = 0;
protected:
    PolkitAgentListener *m_listener;
};
}

#endif
