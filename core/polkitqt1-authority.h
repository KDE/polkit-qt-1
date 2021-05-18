/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_AUTHORITY_H
#define POLKITQT1_AUTHORITY_H

#include "polkitqt1-export.h"
#include "polkitqt1-identity.h"
#include "polkitqt1-subject.h"
#include "polkitqt1-temporaryauthorization.h"
#include "polkitqt1-actiondescription.h"

#include <QObject>
#include <QMetaType>

typedef struct _PolkitAuthority PolkitAuthority;
class QStringList;

/**
 * \namespace PolkitQt1 PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt1
{

typedef QMap<QString, QString> DetailsMap;

/**
 * \class Authority polkitqt1-authority.h Authority
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf@kde.org>
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief Convenience class for Qt/KDE applications
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
class POLKITQT1_EXPORT Authority : public QObject
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
    Q_ENUM(Result)

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

    /** Error codes for the authority class */
    enum ErrorCode {
        /** No error occurred **/
        E_None = 0x00,
        /** Authority cannot be obtained **/
        E_GetAuthority = 0x01,
        /** Authority check failed **/
        E_CheckFailed = 0x02,
        /** Wrong or empty subject was given **/
        E_WrongSubject = 0x03,
        /** Action returned unknown result **/
        E_UnknownResult = 0x04,
        /** Enumerating actions failed **/
        E_EnumFailed = 0x05,
        /** Registration of authentication agent failed **/
        E_RegisterFailed = 0x06,
        /** Unregistration of authentication agent failed **/
        E_UnregisterFailed = 0x07,
        /** Cookie or polkitqt1-identity.handled to the action is empty **/
        E_CookieOrIdentityEmpty = 0x08,
        /** Response of auth agent failed **/
        E_AgentResponseFailed = 0x09,
        /** Revoke temporary authorizations failed **/
        E_RevokeFailed = 0x0A
    };
    Q_ENUM(ErrorCode)

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
    static Authority *instance(PolkitAuthority *authority = nullptr);

    ~Authority() override;

    /**
     * You should always call this method after every action. No action will be allowed
     * if the object is in error state. Use clearError() to clear the error message.
     *
     * \see lastError
     * \see clearError
     *
     * \return \c true if an error occurred, \c false if the library is ready
     */
    bool hasError() const;

    /**
     * \return the code of last error
     */
    ErrorCode lastError() const;

    /**
     * Get detail information about error that occurred.
     *
     * \return detail message
     */
    const QString errorDetails() const;

    /**
     * Use this method to clear the error message.
     */
    void clearError();

    /**
     * Returns the current instance of PolkitAuthority. If you are handling
     * it through Polkit-qt (which is quite likely, since you are calling
     * this function), DO NOT use any PolicyKit API's specific method that
     * modifies the instance on it, unless you're completely aware of what you're doing and
     * of the possible consequencies. Use this instance only to gather information.
     *
     * \return the current PolkitAuthority instance
     */
    PolkitAuthority *polkitAuthority() const;

    /**
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
     * When operation is finished, signal checkAuthorizationFinish is emitted
     * with result of authorization check in its parameter.
     *
     * \see checkAuthorizationSync Synchronous version of this method.
     * \see checkAuthorizationFinished Signal that is emitted when this method finishes.
     * \see checkAuthorizationCancel Use it to cancel execution of this method.
     *
     * \param actionId the Id of the action in question
     * \param subject subject that the action is authorized for (e.g. unix process)
     * \param flags flags that influences the authorization checking
     */
    void checkAuthorization(const QString &actionId, const Subject &subject,
                            AuthorizationFlags flags);

    /**
     * This function does the same as checkAuthorization(const QString&, const
     * Subject&, AuthorizationFlags), but also accepts a
     * DetailsMap parameter. This map can contain key/value pairs that, for
     * example, are used to expand placeholders in polkit authentication
     * messages that are formatted like "Authentication required to access
     * $(device)". For this example, when a key "device" exists in the map,
     * the "$(device)" will be replaced by the corresponding value in the map.
     *
     * \see checkAuthorization(const QString&, const Subject&, AuthorizationFlags) Base version of this method.
     */
    // TODO: merge with checkAuthorization when we decide to break binary compatibility probably in Plasma 6
    void checkAuthorizationWithDetails(
                            const QString &actionId, const Subject &subject,
                            AuthorizationFlags flags, const DetailsMap &details);

    /**
     * Synchronous version of the checkAuthorization method.
     *
     * \param actionId the Id of the action in question
     * \param subject subject that the action is authorized for (e.g. unix process)
     * \param flags flags that influences the authorization checking
     *
     * \see checkAuthorization Asynchronous version of this method.
     */
    Result checkAuthorizationSync(const QString &actionId, const Subject &subject,
                                  AuthorizationFlags flags);

    /**
     * This function does the same as checkAuthorizationSync(const QString&,
     * const Subject&, AuthorizationFlags), but also accepts a DetailsMap
     * parameter.
     *
     * \see checkAuthorization(const QString&, const Subject&, AuthorizationFlags, const DetailsMap&) for a description of the details parameter.
     *
     * \see checkAuthorizationSync(const QString&, const Subject, AuthorizationFlags) Base version of this method.
     */
    Result checkAuthorizationSyncWithDetails(const QString &actionId, const Subject &subject,
                                  AuthorizationFlags flags, const DetailsMap &details);

    /**
     * This method can be used to cancel last authorization check.
     */
    void checkAuthorizationCancel();

    /**
     * Asynchronously retrieves all registered actions.
     *
     * When operation is finished, signal checkAuthorizationFinish is emitted
     * with result of authorization check in its parameter.
     *
     * \see enumerateActionsSync Synchronous version of this method.
     * \see enumerateActionsFinished Signal that is emitted when this method finishes.
     * \see enumerateActionsCancel Use it to cancel execution of this method.
     */
    void enumerateActions();

    /**
     * Synchronously retrieves all registered actions.
     *
     * \see enumerateActions Asynchronous version of this method.
     *
     * \return a list of Action IDs
     */
    ActionDescription::List enumerateActionsSync();

    /**
     * This method can be used to cancel enumeration of actions
     */
    void enumerateActionsCancel();

    /**
     * Registers an authentication agent.
     *
     * \see registerAuthenticationAgentSync Synchronous version of this method.
     * \see registerAuthenticationAgentFinished Signal that is emitted when this method finishes.
     * \see registerAuthenticationAgentCancel Use it to cancel execution of this method.
     *
     * \param subject caller subject
     * \param locale the locale of the authentication agent
     * \param objectPath the object path for the authentication agent
     */
    void registerAuthenticationAgent(const Subject &subject, const QString &locale,
                                     const QString &objectPath);

    /**
     * Registers an authentication agent.
     *
     * \see registerAuthenticationAgent Asynchronous version of this method.
     *
     * \param subject caller subject
     * \param locale the locale of the authentication agent
     * \param objectPath the object path for the authentication agent
     *
     * \return \c true if the Authentication agent has been successfully registered
     *         \c false if the Authentication agent registration failed
    */
    bool registerAuthenticationAgentSync(const Subject &subject, const QString &locale,
                                         const QString &objectPath);

    /**
     * This method can be used to cancel the registration of the authentication agent.
     */
    void registerAuthenticationAgentCancel();

    /**
     * Unregisters an Authentication agent.
     *
     * \see unregisterAuthenticationAgentSync Synchronous version of this method.
     * \see unregisterAuthenticationAgentFinished Signal that is emitted when this method finishes.
     * \see unregisterAuthenticationAgentCancel Use it to cancel execution of this method.
     *
     * \param subject caller subject
     * \param objectPath the object path for the Authentication agent
     *
     * \return \c true if the Authentication agent has been successfully unregistered
     *         \c false if the Authentication agent unregistration failed
     */
    void unregisterAuthenticationAgent(const Subject &subject, const QString &objectPath);

    /**
     * Unregisters an Authentication agent.
     *
     * \see unregisterAuthenticationAgent Asynchronous version of this method.
     *
     * \param subject caller subject
     * \param objectPath the object path for the Authentication agent
     *
     * \return \c true if the Authentication agent has been successfully unregistered
     *         \c false if the Authentication agent unregistration failed
    */
    bool unregisterAuthenticationAgentSync(const Subject &subject, const QString &objectPath);

    /**
     * This method can be used to cancel the unregistration of the authentication agent.
     */
    void unregisterAuthenticationAgentCancel();

    /**
     * Provide response that \p identity successfully authenticated for the authentication request identified by \p cookie.
     *
     * \see authenticationAgentResponseSync Synchronous version of this method.
     * \see authenticationAgentResponseFinished Signal that is emitted when this method finishes.
     * \see authenticationAgentResponseCancel Use it to cancel execution of this method.
     *
     * \param cookie The cookie passed to the authentication agent from the authority.
     * \param identity The identity that was authenticated.
     */
    void authenticationAgentResponse(const QString &cookie, const Identity &identity);

    /**
     * Provide response that \p identity successfully authenticated for the authentication request identified by \p cookie.
     *
     * \see authenticationAgentResponse Asynchronous version of this method.
     *
     * \param cookie The cookie passed to the authentication agent from the authority.
     * \param identity The identity that was authenticated.
     *
     * \return \c true if authority acknowledged the call, \c false if error is set.
     *
    */
    bool authenticationAgentResponseSync(const QString& cookie, const PolkitQt1::Identity& identity);

    /**
     * This method can be used to cancel the authenticationAgentResponseAsync method.
     */
    void authenticationAgentResponseCancel();

    /**
     * Retrieves all temporary action that applies to \p subject.
     *
     * \see enumerateTemporaryAuthorizationsSync Synchronous version of this method.
     * \see enumerateTemporaryAuthorizationsFinished Signal that is emitted when this method finishes.
     * \see enumerateTemporaryAuthorizationsCancel Use it to cancel execution of this method.
     *
     * \param subject the subject to get temporary authorizations for
     *
     */
    void enumerateTemporaryAuthorizations(const Subject &subject);

    /**
     * Retrieves all temporary action that applies to \p subject
     *
     * \see enumerateTemporaryAuthorizations Asynchronous version of this method.
     *
     * \param subject the subject to get temporary authorizations for
     *
     * \note Free all TemporaryAuthorization objects using \p delete operator.
     *
     * \return List of all temporary authorizations
    */
    TemporaryAuthorization::List enumerateTemporaryAuthorizationsSync(const Subject &subject);

    /**
     * This method can be used to cancel the enumerateTemporaryAuthorizationsAsync method.
     */
    void enumerateTemporaryAuthorizationsCancel();

    /**
     * Revokes all temporary authorizations that applies to \p subject
     *
     * \see revokeTemporaryAuthorizationsSync Synchronous version of this method.
     * \see revokeTemporaryAuthorizationsFinished Signal that is emitted when this method finishes.
     * \see revokeTemporaryAuthorizationsCancel Use it to cancel execution of this method.

     * \param subject the subject to revoke temporary authorizations from
     */
    void revokeTemporaryAuthorizations(const Subject &subject);

    /**
     * Revokes all temporary authorizations that applies to \p subject
     *
     * \see revokeTemporaryAuthorizations Asynchronous version of this method.
     *
     * \param subject the subject to revoke temporary authorizations from
     *
     * \return \c true if all temporary authorization were revoked
     *         \c false if the revoking failed
    */
    bool revokeTemporaryAuthorizationsSync(const Subject &subject);

    /**
     * This method can be used to cancel the method revokeTemporaryAuthorizationsAsync.
     */
    void revokeTemporaryAuthorizationsCancel();

    /**
     * Revokes temporary authorization by \p id
     *
     * \see revokeTemporaryAuthorizationSync Synchronous version of this method.
     * \see revokeTemporaryAuthorizationFinished Signal that is emitted when this method finishes.
     * \see revokeTemporaryAuthorizationCancel Use it to cancel execution of this method.
     *
     * \param id the identifier of the temporary authorization
     */
    void revokeTemporaryAuthorization(const QString &id);

    /**
     * Revokes temporary authorization by \p id
     *
     * \see revokeTemporaryAuthorization Asynchronous version of this method.
     *
     * \param id the identifier of the temporary authorization
     *
     * \return \c true if the temporary authorization was revoked
     *         \c false if the revoking failed
    */
    bool revokeTemporaryAuthorizationSync(const QString &id);

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
     * This signal is emitted when asynchronous method checkAuthorization finishes.
     *
     * The argument is the result of authorization.
     */
    void checkAuthorizationFinished(PolkitQt1::Authority::Result);

    /**
     * This signal is emitted when asynchronous method enumerateActions finishes.
     *
     * The argument is the list of all Action IDs.
     */
    void enumerateActionsFinished(PolkitQt1::ActionDescription::List);

    /**
     * This signal is emitted when asynchronous method registerAuthenticationAgent finishes.
     *
     * The argument is \c true  if the Authentication agent has been successfully registered
     *                 \c false if the Authentication agent registration failed
     */
    void registerAuthenticationAgentFinished(bool);

    /**
     * This signal is emitted when asynchronous method unregisterAuthenticationAgent finishes.
     *
     * The argument is \c true  if the Authentication agent has been successfully unregistered
     *                 \c false if the Authentication agent unregistration failed
     */
    void unregisterAuthenticationAgentFinished(bool);

    /**
     * This signal is emitted when asynchronous method authenticationAgentResponse finishes.
     *
     * The argument is \c true if authority acknowledged the call, \c false if error is set.
     */
    void authenticationAgentResponseFinished(bool);

    /**
     * This signal is emitted when asynchronous method enumerateTemporaryAuthorizations finishes.
     *
     * The argument is list of all temporary authorizations.
     *
     * \note Free all TemporaryAuthorization objects using \p delete operator.
     */
    void enumerateTemporaryAuthorizationsFinished(PolkitQt1::TemporaryAuthorization::List);

    /**
     * This signal is emitted when asynchronous method revokeTemporaryAuthorizations finishes.
     *
     * The argument is \c true if all temporary authorizations were revoked
     *                 \c false if the revoking failed
     */
    void revokeTemporaryAuthorizationsFinished(bool);

    /**
     * This signal is emitted when asynchronous method revokeTemporaryAuthorization finishes.
     * \return \c true if the temporary authorization was revoked
     *         \c false if the revoking failed
     */
    void revokeTemporaryAuthorizationFinished(bool);

private:
    explicit Authority(PolkitAuthority *context, QObject *parent = nullptr);

    class Private;
    friend class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void dbusFilter(const QDBusMessage &message))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Authority::AuthorizationFlags)

}

#endif
