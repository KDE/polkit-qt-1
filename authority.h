/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
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

#ifndef POLKIT_QT_AUTHORITY_H
#define POLKIT_QT_AUTHORITY_H

#include "export.h"
#include "identity.h"
#include "subject.h"
#include "temporaryauthorization.h"

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <polkit/polkit.h>

typedef struct _PolKitAuthority PolKitAuthority;

/**
 * \namespace PolkitQt PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt
{

/**
 * \class Authority authority.h Authority
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf54321@gmail.com>
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief Convenience class for Qt/KDE aplications
 *
 * This class is a singleton that provides makes easy the usage
 * of PolKitAuthority. It emits configChanged()
 * whenever PolicyKit files change (e.g. the PolicyKit.conf
 * or .policy files) or when ConsoleKit reports activities changes.
 *
 * \note This class is a singleton, its constructor is private.
 * Call Authority::instance() to get an instance of the Authority object.
 * Do not delete Authority::instance(), cleanup will be done automatically.
 */
class POLKIT_QT_EXPORT Authority : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Authority)
public:
    enum Result {
        /** Result unknown */
        Unknown = 0x00,
        /** The subject is authorized for the specified action  */
        Yes = 0x01,
        /** The subject is not authorized for the specified action  */
        No = 0x02,
        /** The subject is authorized if more information is provided */
        Challenge = 0x03
    };

    enum AuthorizationFlag {
        /** No flags set **/
        None = 0x00,
        /** If the subject can obtain the authorization through authentication,
        * and an authentication agent is available, then attempt to do so.
        *
        * Note, this means that the method used for checking authorization is likely
        * to block for a long time. **/
        AllowUserInteraction = 0x01
    };
    Q_DECLARE_FLAGS(AuthorizationFlags, AuthorizationFlag)

    /**
     * \brief Returns the instance of Authority
     *
     * Returns the current instance of Authority. Call this function whenever
     * you need to access the Authority class.
     *
     * \note Authority is a singleton. Memory is handled by polkit-qt, so you just
     * need to call this function to get a working instance of Authority.
     * Don't delete the object after having used it.
     *
     * \param authority use this if you want to set an explicit PolkitAuthority. If you
     *                don't know what this implies, simply ignore the parameter. In case
     *                you want to use it, be sure of streaming it the first time you call
     *                this function, otherwise it will have no effect.
     *
     * \return The current authority instance
     */
    static Authority* instance(PolkitAuthority *authority = 0);

    ~Authority();

    /**
     * You should always call this method first, since if an error is detected,
     * the library will retry to initialize itself.
     *
     * \return true if the library is ready, false if an error occurred
     */
    bool hasError() const;

    /**
     * \return the last error message
     */
    QString lastError() const;

    /**
     * Returns the current instance of PolkitAuthority. If you are handling
     * it through Polkit-qt (which is quite likely, since you are calling
     * this function), DO NOT use any PolicyKit API's specific method that
     * modifies the instance on it, unless you're completely aware of what you're doing and
     * of the possible consequencies. Use this instance only to gather information.
     *
     * \return the current PolkitAuthority instance
     */
    PolkitAuthority *getPolkitAuthority() const;

    /**
     * TODO: rewrite, finish!
     * This function should be used by mechanisms (e.g.: helper applications).
     * It returns the action should be carried out, so if the caller was
     * actually authorized to perform it. The result is in form of a Result, so that
     * you can have more control over the whole process, and detect an eventual error.
     * Most of the times you simply want to check if the result is == to \c Result::Yes,
     * if you don't have specific needs.
     *
     * It is CRITICAL that you call this function
     * and check what it returns before doing anything in your helper, since otherwise
     * you could be actually performing an action from an unknown or unauthorized caller.
     *
     * \param actionId the Id of the action in question
     * \param subject ...
     * \param flags
     *
     * \return \c Result::Yes if the caller is authorized and the action should be performed
     *         \c otherwise if the caller was not authorized and the action should not be performed,
     *                      or an error has occurred
     *
     */
    Result checkAuthorization(const QString &actionId, Subject *subject,
                              AuthorizationFlags flags);

    /**
     * Asynchronous version of the checkAuthorization method.
     * When operation is finished, signal checkAuthorizationFinish is emitted
     * with result of authorization chech in its parameter.
     *
     * \param actionId the Id of the action in question
     * \param subject ...
     * \param flags
     */
    void checkAuthorizationAsync(const QString &actionId, Subject *subject,
                                 AuthorizationFlags flags);
    /**
     * This method can be used to cancel last authorization check.
     */
    void checkAuthorizationCancel();

    /**
      * Synchronously retrieves all registered actions.
      *
      * \return a list of Action IDs
      */
    QStringList enumerateActions();

    /**
     * Asynchronous version of method enumerateAction.
     */
    void enumerateActionsAsync();

    /**
     * This method can be used to cancel enumeration of actions
     */
    void enumerateActionsCancel();

    /**
     * Registers an authentication agent.
     *
     * \param subject caller subject
     * \param locale the locale of the authentication agent
     * \param objectPath the object path for the authentication agent
     *
     * \return \c true if the Authentication agent has been successfully registered
     *         \c false if the Authentication agent registration failed
    */
    bool registerAuthenticationAgent(Subject *subject, const QString &locale,
                                     const QString &objectPath);

    /**
     * Asynchronous version of method registerAuthenticationAgent.
     *
     * \param subject caller subject
     * \param locale the locale of the authentication agent
     * \param objectPath the object path for the authentication agent
     */
    void registerAuthenticationAgentAsync(Subject *subject, const QString &locale,
                                          const QString &objectPath);

    /**
     * This method can be used to cancel the registration of the authentication agent.
     */
    void registerAuthenticationAgentCancel();


    /**
     * Unregisters an Authentication agent.
     *
     * \param subject caller subject
     * \param objectPath the object path for the Authentication agent
     *
     * \return \c true if the Authentication agent has been successfully unregistered
     *         \c false if the Authentication agent unregistration failed
    */
    bool unregisterAuthenticationAgent(Subject *subject, const QString &objectPath);

    /**
     * Asynchronous version of method unregisterAuthenticationAgent.
     * \param subject caller subject
     * \param objectPath the object path for the Authentication agent
     *
     * \return \c true if the Authentication agent has been successfully unregistered
     *         \c false if the Authentication agent unregistration failed
     */
    void unregisterAuthenticationAgentAsync(Subject *subject, const QString &objectPath);

    /**
     * This method can be used to cancel the unregistration of the authentication agent.
     */
    void unregisterAuthenticationAgentCancel();

    /**
      * Provide response that \p identity successfully authenticated for the authentication request identified by \p cookie.
      * \param cookie The cookie passed to the authentication agent from the authority.
      * \param identity The identity that was authenticated.
      *
      * \return \c true if authority acknowledged the call, \c false if error is set.
      *
    */
    bool authenticationAgentResponse(const QString & cookie, Identity * identity);

    /**
     * Asynchronous version of method authenticationAgentResponse.
     *
     * \param cookie The cookie passed to the authentication agent from the authority.
     * \param identity The identity that was authenticated.
     */
    void authenticationAgentResponseAsync(const QString & cookie, Identity * identity);

    /**
     * This method can be used to cancel the authenticationAgentResponseAsync method.
     */
    void authenticationAgentResponseCancel();

    /**
     * Retrieves all temporary action that applies to \p subject
     *
     * \param subject the subject to get temporary authorizations for
     *
     * \return List of all temporary authorizations
    */
    QList<TemporaryAuthorization *> enumerateTemporaryAuthorizations(Subject *subject);

    /**
     * Asynchronous version of method enumerateTemporaryAuthorizations.
     *
     * \param subject the subject to get temporary authorizations for
     *
     */
    void enumerateTemporaryAuthorizationsAsync(Subject *subject);

    /**
     * This method can be used to cancel the enumerateTemporaryAuthorizationsAsync method.
     */
    void enumerateTemporaryAuthorizationsCancel();

    /**
     * Revokes all temporary authorizations that applies to \p subject
     *
     * \param subject the subject to revoke temporary authorizations from
     *
     * \return \c true if all temporary authorization were revoked
     *         \c false if the revoking failed
    */
    bool revokeTemporaryAuthorizations(Subject *subject);

    /** Asynchronous version of method revokeTemporaryAuthorizations.
     *
     * \param subject the subject to revoke temporary authorizations from
     */
    void revokeTemporaryAuthorizationsAsync(Subject *subject);

    /**
     * This method can be used to cancel the method revokeTemporaryAuthorizationsAsync.
     */
    void revokeTemporaryAuthorizationsCancel();

    /**
     * Revokes temporary authorization by \p id
     *
     * \param id the identifier of the temporary authorization
     *
     * \return \c true if the temporary authorization was revoked
     *         \c false if the revoking failed
    */
    bool revokeTemporaryAuthorization(const QString &id);

    /** Asynchronous version of method revokeTemporaryAuthorization.
     *
     * \param id the identifier of the temporary authorization
     */
    void revokeTemporaryAuthorizationAsync(const QString &id);

    /**
     * This method can be used to cancel the method revokeTemporaryAuthorizationAsync.
     */
    void revokeTemporaryAuthorizationCancel();

Q_SIGNALS:
    /**
     * This signal will be emitted when a configuration
     * file gets changed (e.g. /etc/PolicyKit/PolicyKit.conf or
     * .policy files).
     * Connect to this signal if you want to track down
     * actions.
     */
    void configChanged();

    /**
     * This signal is emitted when ConsoleKit configuration
     * changes. This might happen when a session becomes active
     * or inactive.
     *
     * If you want to track your actions directly you should
     * connect to this signal, as this might change the return value
     * PolicyKit will give you.
     *
     * \note If you use Action you'll probably prefer to
     * use the dataChanged() signal to track Action changes.
     */
    void consoleKitDBChanged();

    /**
     * This signal is emitted when asynchronous method checkAuthorizationAsync finishes.
     *
     * The argument is the result of authorization.
     */
    void checkAuthorizationFinished(PolkitQt::Authority::Result);

    /**
     * This signal is emitted when asynchronous method enumerateActionsAsync finishes.
     *
     * The argument is the list of all Action IDs.
     */
    void enumerateActionsFinished(QStringList);

    /**
     * This signal is emitted when asynchronous method registerAuthenticationAgentAsync finishes.
     *
     * The argument is \c true  if the Authentication agent has been successfully registered
     *                 \c false if the Authentication agent registration failed
     */
    void registerAuthenticationAgentFinished(bool);

    /**
     * This signal is emitted when asynchronous method unregisterAuthenticationAgentAsync finishes.
     *
     * The argument is \c true  if the Authentication agent has been successfully unregistered
     *                 \c false if the Authentication agent unregistration failed
     */
    void unregisterAuthenticationAgentFinished(bool);

    /**
     * This signal is emitted when asynchronous method authenticationAgentResponseAsync finishes.
     *
     * The argument is \c true if authority acknowledged the call, \c false if error is set.
     */
    void authenticationAgentResponseFinished(bool);

    /**
     * This signal is emmited when asynchronous method enumerateTemporaryAuthorizations finishes.
     *
     * The argument is list of all temporary authorizations.
     */
    void enumerateTemporaryAuthorizationsFinished(QList<TemporaryAuthorization *>);

    /**
     * This signal is emmited when asynchronous method revokeTemporaryAuthorizationsAsync finishes.
     *
     * The argument is \c true if all temporary authorizations were revoked
     *                 \c false if the revoking failed
     */
    void revokeTemporaryAuthorizationsFinished(bool);

    /**
     * This signal is emmited when asynchronous method revokeTemporaryAuthorizationAsync finishes.
     * \return \c true if the temporary authorization was revoked
     *         \c false if the revoking failed
     */
    void revokeTemporaryAuthorizationFinished(bool);

private:
    explicit Authority(PolkitAuthority *context, QObject *parent = 0);

    class Private;
    friend class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void dbusFilter(const QDBusMessage &message))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PolkitQt::Authority::AuthorizationFlags)

}

Q_DECLARE_METATYPE(PolkitQt::Authority::Result);

#endif
