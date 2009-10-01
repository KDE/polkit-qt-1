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
#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE
#include <polkitagent/polkitagent.h>

/**
 * \namespace PolkitQtAgent PolkitQtAgent
 *
 * \brief Namespace wrapping Polkit-Qt Agent classes
 *
 * This namespace wraps all Polkit-Qt Agent classes.
 */
namespace PolkitQtAgent
{

/**
 * \class Session session.h Session
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * This class is interface for interacting with native
 * authentication system for obtaining authorizations.
 *
 */
class POLKIT_QT_EXPORT Session : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a new authentication session.
     *
     * \param identity The identity to authenticate
     * \param cookie The cookie obtained from the PolicyKit daemon
     */
    Session(PolkitQt::Identity *identity, const QString &cookie);

    /**
     * Initiate the authentication session.
     *
     * Use cancel() to cancel the session.
     */
    void initiate();

    /**
     * Method for providing response to requests recieved via request signal.
     *
     * \param response Response from the user, typically a password
     */
    void response(const QString &response);

    /**
     * Cancel the authentication session.
     * This will emit the "completed" signal.
     */
    void cancel();

signals:
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
    static void _completed(PolkitAgentSession *s, gboolean gained_authorization, gpointer user_data);
    static void _request(PolkitAgentSession *s, gchar *request, gboolean echo_on, gpointer user_data);
    static void _showError(PolkitAgentSession *s, gchar *text, gpointer user_data);
    static void _showInfo(PolkitAgentSession *s, gchar *text, gpointer user_data);

    PolkitAgentSession *m_polkitAgentSession;
};

}

#endif // SESSION_H
