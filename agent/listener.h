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
class POLKIT_QT_EXPORT Listener : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Listener)
public:
    /**
     * \brief Constructor of Listener class
     */
    Listener(QObject *parent = 0);

    /**
     * \brief Constuctor of Listener class from PolkitAgentListener
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param listener Pointer to the PolkitAgentListener
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
    bool registerListener(PolkitQt::Subject *subject, const QString &objectPath);

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
     * the session needs to prove he is one of the identities listed int \p identities.
     *
     * \note You have to reimplement this method in the subclass.
     *
     * \param actionId The action to authenticate for
     * \param message The message to present to the user
     * \param iconName The name of the icon which is representing the action
     * \param details Details describing the action
     * \param cookie The cookie for the authentization request
     * \param identities A list of Identity object that the user can choose to authenticate as
     * \param result This AsyncResult must be completed by using complete() method when the
     *        authentication is done. You can use Session class that will handle that automatically
     *        when you pass the result to its constructor.
     */
    virtual void initiateAuthentication(const QString &actionId,
                                        const QString &message,
                                        const QString &iconName,
                                        PolkitQt::Details *details,
                                        const QString &cookie,
                                        QList<PolkitQt::Identity *> identities,
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

Q_SIGNALS:
    /**
     * \brief Emit this signal in the subclass when authentication finishes.
     *
     * If you use the Session class, handler of its completed signal is the right place.
     */
    void finished();

private:
    ListenerPrivate * const d;
};
}

#endif
