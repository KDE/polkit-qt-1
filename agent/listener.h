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

#include "export.h"

#include <QtCore/QObject>

#include "session.h"

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

typedef struct _PolkitAgentListener PolkitAgentListener;

namespace PolkitQt1 {
    class Subject;
    class Identity;
    class Details;
}

namespace PolkitQt1
{

namespace Agent
{

class ListenerPrivate;
/**
 * \class Listener listener.h Listener
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief Listener is abstract class used for implementing authentication agents.
 *
 * To implement an authentication agent, just subclass this class and implement
 * virtual functions initiateAuthentication, initiateAuthenticationFinish
 * and cancelAuthentication.
 *
 * You can also use Session class to authenticate users however it isn't required.
 * \sa Session
 */
class POLKITQT1_EXPORT Listener : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Listener)
public:
    /**
     * \brief Constructor of Listener class
     */
    Listener(QObject *parent = 0);

    /**
     * \brief Constructor of Listener class from PolkitAgentListener
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param listener Pointer to the PolkitAgentListener
     * \param parent
     */
    explicit Listener(PolkitAgentListener *listener, QObject *parent = 0);

    virtual ~Listener();

    /**
     * \brief Registers listener with polkit daemon as an authentication agent for \p subject.
     *
     * This is implemented by registering a DBus object at \p objectPath on the unique
     * name assigned by the system message bus.
     *
     * Whenever the polkit daemon needs to authenticate a processes that is related to \p subject,
     * the methods initiateAuthentication and initiateAuthenticationFinish will be evoked.
     *
     * \param subject Subject that listener will be registered for
     * \param objectPath DBus object path
     * \return \c True if the listener has been registered, \c False otherwise
     */
    bool registerListener(PolkitQt1::Subject *subject, const QString &objectPath);

    /**
     * \brief Returns pointer to the PolkitAgentListener.
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \return PolkitAgentListener
     */
    const PolkitAgentListener *listener();

public Q_SLOTS:
    /**
     * \brief Initiate authentication for the action
     *
     * This method will be called on a registered authentication agent when the user owning
     * the session needs to prove he is one of the identities listed in \p identities.
     *
     * \note You have to reimplement this method in the subclass.
     *
     * \param actionId The action to authenticate for
     * \param message The message to present to the user
     * \param iconName The name of the icon which is representing the action
     * \param details Details describing the action
     * \param cookie The cookie for the authentization request
     * \param identities A list of Identity object that the user can choose to authenticate as
     * \param result This AsyncResult MUST be completed by using complete() method when the
     *        authentication is done. You can pass it to the constructor of the Session class
     *        and then call session->result()->complete() to mark the action as done.
     */
    virtual void initiateAuthentication(const QString &actionId,
                                        const QString &message,
                                        const QString &iconName,
                                        PolkitQt1::Details *details,
                                        const QString &cookie,
                                        QList<PolkitQt1::Identity *> identities,
                                        AsyncResult *result) = 0;

    /**
     * TODO: Is this method really required ?
     * \brief Finishes an authentication request from the polkit daemon.
     *
     * \note You have to reimplement this method in the subclass.
     *
     * \see initiateAuthentication
     */
    virtual bool initiateAuthenticationFinish() = 0;

    /**
     * TODO: Is this method really required ?
     * \brief Cancels an authentication request from the polkit daemon.
     *
     * \note You have to reimplement this method in the subclass.
     *
     * \see initiateAuthentication
     */
    virtual void cancelAuthentication() = 0;

private:
    ListenerPrivate * const d;
};
}

}

#endif
