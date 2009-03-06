/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
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

#ifndef POLKIT_QT_ACTION_H
#define POLKIT_QT_ACTION_H

#include "export.h"

#include <QtGui/QAction>
#include <QtCore/QString>

#include <polkit/polkit.h>

namespace PolkitQt
{

/**
 * \class Action action.h Action
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf54321@gmail.com>
 *
 * \brief Class used to manage actions
 *
 * This class is an interface around PolicyKit Actions.
 * By using this class, you are able to track the result of a
 * given action.
 *
 * Most of the times, you would want to use this class combined
 * with a QAbstractButton. In this case, you can use the more 
 * comfortable ActionButton class that manages button's properties
 * update for you.
 * 
 * \see ActionButton
 */
class POLKIT_QT_EXPORT Action : public QAction
{
    Q_OBJECT
public:
    /**
     * Constructs a new Action item
     *
     * \param actionId the PolicyKit action Id (e.g.: org.freedesktop.policykit.read)
     * \param parent the object parent
     */
    Action(const QString &actionId, QObject *parent);
    ~Action();

Q_SIGNALS:
    /**
     * Emitted when the PolicyKit result (PolKitResult)
     * for the given action or the internal data changes
     * (i.e. the user called one of the set methods).
     * You should connect to this signal if you want
     * to track these changes.
     */
    void dataChanged();

    /**
     * Emitted when using this class as a proxy
     * for a given action, It's only emitted if the
     * activate() slot is called and the auth permits
     * the action
     *
     * \see activate()
     */
    void activated();

public Q_SLOTS:
    /**
     * Use this slot if you want to activate
     * the action. activated() will be emitted
     * if the action gets authorized.
     *
     * \note This will call the auth dialog
     * if needed. Only use the return value if
     * you want a sync behavior.
     *
     * \param winId The window id of the parent window,
     *              used for the dialog.
     *
     * \return \c true if the caller can do the action
     *
     * \see activated()
     */
    bool activate(WId winId = 0);

    /**
     * Defines the checked state. The opposite state will
     * trigger authentication for this actions. For example, if
     * you set this to \c true, when the action's checked state
     * will become \c false, the authentication will be triggered.
     *
     * \param checked the new checked state
     */
    void setChecked(bool checked);

    /**
     * This method can be used to revoke the authorization
     * obtained for this action.
     */
    void revoke();

public:
    /**
     * Changes the action being tracked
     *
     * \param actionId The new action ID
     */
    void setPolkitAction(const QString &actionId);

    /**
     * Returns the PolKitAction pointer associated
     * with this Action. Use this only if you know
     * what you're doing
     *
     * \note Unref this pointer after usage with
     * polkit_action_unref (), to avoid leaking.
     * Also never use this directly otherwise you
     * will not be able to unref it.
     *
     * \return The underlying PolKitAction item
     *
     */
    PolKitAction* polkitAction() const;

    /**
     * Returns the current action ID.
     *
     * \return The action ID
     *
     */
    QString       actionId() const;

    /**
     * This function allows you to override PolicyKit settings
     * about visibility. You can set this to \c false to force
     * invisibility without caring about what PolicyKit reports.
     * 
     * \param value \c false if you want to force invisibility
     */
    void setMasterVisible(bool value);

    /**
     * \see setMasterVisible
     */
    bool masterVisible() const;

    /**
     * This function allows you to override PolicyKit settings
     * about availability. You can set this to \c false to force
     * the action being disabled without caring about what PolicyKit reports.
     * 
     * \param value \c false if you want to force the action to be disabled
     */
    void setMasterEnabled(bool value);

    /**
     * \see setMasterEnabled
     */
    bool masterEnabled() const;

    /**
     * Sets the text for the current action. This will
     * be shown in all four states.
     * \param text the new text for the action
     */
    void setText(const QString &text);

    /**
     * Sets the tooltip for the current action. This will
     * be shown in all four states.
     * \param toolTip the new tooltip for the action
     */
    void setToolTip(const QString &toolTip);

    /**
     * Sets the whatsthis for the current action. This will
     * be shown in all four states.
     * \param whatsThis the new whatsthis for the action
     */
    void setWhatsThis(const QString &whatsThis);

    /**
     * Sets the icon for the current action. This will
     * be shown in all four states.
     * \note You need to pass a QIcon here. You can easily
     *       create one from a Pixmap, or pass a KIcon
     * \param icon the new icon for the action
     */
    void setIcon(const QIcon &icon);

    /**
     * This function is used to modify the visibility of the
     * action in case PolicyKit evaluates the result as 'no' 
     * because the user has a self-granted negative authorization.
     *
     * \param value \c true if the action should be visible
     *              \c false if the action shouldn't be visible
     */
    void setSelfBlockedVisible(bool value);

    /**
     * \see setSelfBlockedVisible
     */
    bool selfBlockedVisible() const;

    /**
     * This function is used to modify the availability of the
     * action in case PolicyKit evaluates the result as 'no' 
     * because the user has a self-granted negative authorization.
     *
     * \param value \c true if the action should be enabled
     *              \c false if the action should be disabled
     */
    void setSelfBlockedEnabled(bool value);

    /**
     * \see setSelfBlockedEnabled
     */
    bool selfBlockedEnabled() const;

    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'no' 
     * because the user has a self-granted negative authorization.
     *
     * \param text the text the action should display in this case
     */
    void setSelfBlockedText(const QString &text);

    /**
     * \see setSelfBlockedText
     */
    QString selfBlockedText() const;

    /**
     * This function is used to modify the tooltip of the
     * action in case PolicyKit evaluates the result as 'no' 
     * because the user has a self-granted negative authorization.
     *
     * \param toolTip the tooltip the action should display in this case
     */
    void setSelfBlockedToolTip(const QString &toolTip);

    /**
     * \see setSelfBlockedToolTip
     */
    QString selfBlockedToolTip() const;

    /**
     * This function is used to modify the whatsthis of the
     * action in case PolicyKit evaluates the result as 'no' 
     * because the user has a self-granted negative authorization.
     *
     * \param whatsThis the whatsthis the action should display in this case
     */
    void setSelfBlockedWhatsThis(const QString &whatsThis);

    /**
     * \see setSelfBlockedWhatsThis
     */
    QString selfBlockedWhatsThis() const;

    /**
     * This function is used to modify the icon of the
     * action in case PolicyKit evaluates the result as 'no' 
     * because the user has a self-granted negative authorization.
     *
     * \see setIcon
     *
     * \param icon the icon the action should display in this case
     */
    void setSelfBlockedIcon(const QIcon &icon);

    /**
     * \see setSelfBlockedIcon
     */
    QIcon selfBlockedIcon() const;


    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'no'
     *
     * \param value \c true if the action should be visible
     *              \c false if the action shouldn't be visible
     */
    void setNoVisible(bool value);

    /**
     * \see setNoVisible
     */
    bool noVisible() const;

    /**
     * This function is used to modify the availability of the
     * action in case PolicyKit evaluates the result as 'no'
     *
     * \param value \c true if the action should be enabled
     *              \c false if the action should be disabled
     */
    void setNoEnabled(bool value);

    /**
     * \see setNoEnabled
     */
    bool noEnabled() const;

    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'no'
     *
     * \param text the text the action should display in this case
     */
    void setNoText(const QString &text);

    /**
     * \see setNoText
     */
    QString noText() const;

    /**
     * This function is used to modify the tooltip of the
     * action in case PolicyKit evaluates the result as 'no'
     *
     * \param toolTip the tooltip the action should display in this case
     */
    void setNoToolTip(const QString &toolTip);

    /**
     * \see setNoToolTip
     */
    QString noToolTip() const;

    /**
     * This function is used to modify the whatsthis of the
     * action in case PolicyKit evaluates the result as 'no'
     *
     * \param whatsThis the whatsthis the action should display in this case
     */
    void setNoWhatsThis(const QString &whatsThis);

    /**
     * \see setNoWhatsThis
     */
    QString noWhatsThis() const;

    /**
     * This function is used to modify the icon of the
     * action in case PolicyKit evaluates the result as 'no'
     *
     * \see setIcon
     *
     * \param icon the icon the action should display in this case
     */
    void setNoIcon(const QIcon &icon);

    /**
     * \see setNoIcon
     */
    QIcon noIcon() const;


    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'Auth'
     *
     * \param value \c true if the action should be visible
     *              \c false if the action shouldn't be visible
     */
    void setAuthVisible(bool value);

    /**
     * \see setAuthVisible
     */
    bool authVisible() const;

    /**
     * This function is used to modify the availability of the
     * action in case PolicyKit evaluates the result as 'Auth'
     *
     * \param value \c true if the action should be enabled
     *              \c false if the action should be disabled
     */
    void setAuthEnabled(bool value);

    /**
     * \see setAuthEnabled
     */
    bool authEnabled() const;

    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'Auth'
     *
     * \param text the text the action should display in this case
     */
    void setAuthText(const QString &text);

    /**
     * \see setAuthText
     */
    QString authText() const;

    /**
     * This function is used to modify the tooltip of the
     * action in case PolicyKit evaluates the result as 'Auth'
     *
     * \param toolTip the tooltip the action should display in this case
     */
    void setAuthToolTip(const QString &toolTip);

    /**
     * \see setAuthToolTip
     */
    QString authToolTip() const;

    /**
     * This function is used to modify the whatsthis of the
     * action in case PolicyKit evaluates the result as 'Auth'
     *
     * \param whatsThis the whatsthis the action should display in this case
     */
    void setAuthWhatsThis(const QString &whatsThis);

    /**
     * \see setAuthWhatsThis
     */
    QString authWhatsThis() const;

    /**
     * This function is used to modify the icon of the
     * action in case PolicyKit evaluates the result as 'Auth'
     *
     * \see setIcon
     *
     * \param icon the icon the action should display in this case
     */
    void setAuthIcon(const QIcon &icon);

    /**
     * \see setAuthIcon
     */
    QIcon authIcon() const;


    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'Yes'
     *
     * \param value \c true if the action should be visible
     *              \c false if the action shouldn't be visible
     */
    void setYesVisible(bool value);

    /**
     * \see setYesVisible
     */
    bool yesVisible() const;

    /**
     * This function is used to modify the availability of the
     * action in case PolicyKit evaluates the result as 'Yes'
     *
     * \param value \c true if the action should be enabled
     *              \c false if the action should be disabled
     */
    void setYesEnabled(bool value);

    /**
     * \see setYesEnabled
     */
    bool yesEnabled() const;

    /**
     * This function is used to modify the text of the
     * action in case PolicyKit evaluates the result as 'Yes'
     *
     * \param text the text the action should display in this case
     */
    void setYesText(const QString &text);

    /**
     * \see setYesText
     */
    QString yesText() const;

    /**
     * This function is used to modify the tooltip of the
     * action in case PolicyKit evaluates the result as 'Yes'
     *
     * \param toolTip the tooltip the action should display in this case
     */
    void setYesToolTip(const QString &toolTip);

    /**
     * \see setYesToolTip
     */
    QString yesToolTip() const;

    /**
     * This function is used to modify the whatsthis of the
     * action in case PolicyKit evaluates the result as 'Yes'
     *
     * \param whatsThis the whatsthis the action should display in this case
     */
    void setYesWhatsThis(const QString &whatsThis);

    /**
     * \see setYesWhatsThis
     */
    QString yesWhatsThis() const;

    /**
     * This function is used to modify the icon of the
     * action in case PolicyKit evaluates the result as 'Yes'
     *
     * \see setIcon
     *
     * \param icon the icon the action should display in this case
     */
    void setYesIcon(const QIcon &icon);

    /**
     * \see setYesIcon
     */
    QIcon yesIcon() const;

    /**
     * This function sets the process id of the target that
     * should receive the authorization. Set this to 0 to set
     * the current process as the target.
     *
     * \param pid The target process id; 0 if it is the current process
     */
    void setTargetPID(pid_t pid);

    /**
     * \see setTargetPID
     */
    pid_t targetPID();

    /**
     * This method can be used to compute the PolKitResult
     * of a given action
     * \param action the PolKitAction in question
     * \param pid the process id that the action can be requested for
     */
    static PolKitResult computePkResultDirect(PolKitAction *action, pid_t pid);

    /**
     * This method can be used to check the if the current action
     * can be performed (i.e. PolKitResult is YES).
     * \note This method does not call the authentication dialog, use
     *       activate() instead
     * \return \c true if the action can be performed
     */
    bool canDoAction() const;

    /**
     * This method compares a PolicyKit action Id with the
     * current one of the object.
     *
     * \see actionId()
     *
     * \param actionId the action Id to compare
     *
     * \return \c true if the actionId is the same as this object's one
     */
    bool is(const QString &actionId) const;

private Q_SLOTS:
    void configChanged();

private:
    static polkit_bool_t auth_foreach_revoke(PolKitAuthorizationDB *authdb,
                                             PolKitAuthorization   *auth,
                                             void                  *user_data);
    class Private;
    Private * const d;
};

}

#endif
