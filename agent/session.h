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

#ifndef SESSION_H
#define SESSION_H

#include <QtCore/QObject>
#include "identity.h"

typedef struct _GSimpleAsyncResult GSimpleAsyncResult;
typedef struct _PolkitAgentSession PolkitAgentSession;

namespace PolkitQt1
{

/**
 * \namespace Agent Agent
 *
 * \brief Namespace wrapping Polkit-Qt Agent classes
 *
 * This namespace wraps all Polkit-Qt Agent classes.
 */

namespace Agent
{

/**
 * \internal
 * \brief Encapsulation of GSimpleAsyncResult to QObject class
 */
class POLKITQT1_EXPORT AsyncResult : public QObject
{
    Q_OBJECT
public:
    explicit AsyncResult(GSimpleAsyncResult *result);
    virtual ~AsyncResult();

    /**
     * \brief Mark the action that is tied to this result as completed.
     */
    void complete();

    /**
     * \brief Sets an error for the asynchronous result.
     * Method complete() must be called anyway.
     *
     * \param text text of the error message
     */
    void setError(const QString &text);

private:
    class Private;
    Private * const d;
};

/**
 * \class Session session.h Session
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * This class is interface for interacting with native
 * authentication system for obtaining authorizations.
 *
 */
class POLKITQT1_EXPORT Session : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a new authentication session.
     *
     * \param identity The identity to authenticate
     * \param cookie The cookie obtained from the PolicyKit daemon
     * \param result Result of the authentication action. Must be finished using complete() method.
     * \param parent
     */
    Session(PolkitQt1::Identity *identity, const QString &cookie, AsyncResult *result = 0, QObject *parent = 0);

    /**
     * Create a new authentication session from PolkitAgentSession object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkAgentSession PolkitAgentSession object
     * \param parent
     */
    explicit Session(PolkitAgentSession *pkAgentSession, QObject *parent = 0);

    /**
     * Destroy authentication session.
     */
    ~Session();

    /**
     * Initiate the authentication session.
     *
     * Use cancel() to cancel the session.
     */
    void initiate();

    /**
     * Method for providing response to requests received via request signal.
     *
     * \param response Response from the user, typically a password
     */
    void response(const QString &response);

    /**
     * Cancel the authentication session.
     * This will emit the completed() signal.
     */
    void cancel();

    /**
     * Get AsyncResult that can be used to finish authentication operation
     *
     * \return AsyncResult object or NULL if it is not set
     */
    AsyncResult *result();

Q_SIGNALS:
    /**
     * This signal will be emitted when the authentication
     * session has been completed or cancelled.
     *
     * \param gainedAuthorization \c True if authorization was successfully obtained.
     */
    void completed(bool gainedAuthorization);

    /**
     * This signal will be emitted when user is requested to answer a question.
     *
     * \param request The request to show the user, e.g. "name: " or "password: ".
     * \param echo \c True if the response to the request SHOULD be echoed on the screen,
     *             \c False if the response MUST NOT be echoed to the screen.
     */
    void request(const QString &request, bool echo);

    /**
     * This signal will be emitted when there is information
     * related to an error condition to be displayed to the user.
     *
     * \param text An error string to display to the user.
     */
    void showError(const QString &text);

    /**
     * This signal will be emitted when there is information
     * to be displayed to the user.
     *
     * \param text A string to be displayed to the user.
     */
    void showInfo(const QString &text);

private:
    class Private;
    Private * const d;
};

}

}

#endif // SESSION_H
