#ifndef SESSION_H
#define SESSION_H


#include <QtCore/QObject>
#include "identity.h"
#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE
#include <polkitagent/polkitagent.h>

/**
 * \namespace PolkitQtAgent PolkitQtAgen
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
class Session : public QObject
{
    Q_OBJECT
    PolkitAgentSession *m_polkitAgentSession;
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

protected:
    static void _completed(PolkitAgentSession *s, gboolean gained_authorizaition, gpointer user_data);
    static void _request(PolkitAgentSession *s, gchar *request, gboolean echo_on, gpointer user_data);
    static void _showError(PolkitAgentSession *s, gchar *text, gpointer user_data);
    static void _showInfo(PolkitAgentSession *s, gchar *text, gpointer user_data);
};

}

#endif // SESSION_H
