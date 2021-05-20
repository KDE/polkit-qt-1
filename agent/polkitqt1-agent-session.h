/*
    This file is part of the PolKit1-qt project
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_AGENT_SESSION_H
#define POLKITQT1_AGENT_SESSION_H

#include <QObject>
#include "polkitqt1-identity.h"
#include "polkitqt1-agent-export.h"

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
class POLKITQT1_AGENT_EXPORT AsyncResult
{
public:
    explicit AsyncResult(GSimpleAsyncResult *result);
    virtual ~AsyncResult();

    /**
     * \brief Mark the action that is tied to this result as completed.
     */
    void setCompleted();

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
 * \class Session polkitqt1-agent-session.h Session
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * This class is interface for interacting with native
 * authentication system for obtaining authorizations.
 *
 */
class POLKITQT1_AGENT_EXPORT Session : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Session)
public:
    /**
     * Create a new authentication session.
     *
     * \param identity The identity to authenticate
     * \param cookie The cookie obtained from the PolicyKit daemon
     * \param result Result of the authentication action. Must be finished using complete() method.
     * \param parent
     */
    Session(const PolkitQt1::Identity& identity, const QString &cookie, AsyncResult *result = nullptr, QObject *parent = nullptr);

    /**
     * Create a new authentication session from PolkitAgentSession object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkAgentSession PolkitAgentSession object
     * \param parent
     */
    explicit Session(PolkitAgentSession *pkAgentSession, QObject *parent = nullptr);

    /**
     * Destroy authentication session.
     */
    ~Session() override;

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
    void setResponse(const QString &response);

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
     * polkitqt1-agent-session.has been completed or cancelled.
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
