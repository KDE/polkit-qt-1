/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf@kde.org>
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

namespace PolkitQt
{

/**
 * \class Action action.h Action
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf@kde.org>
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
    Q_DISABLE_COPY(Action)
public:

    enum State {
        None = 0,
        SelfBlocked = 1,
        Yes = 2,
        No = 4,
        Auth = 8,
        // Future usage = 16,
        // Future usage = 32,
        // Future usage = 64,
        // Future usage = 128,
        // Future usage = 256,
        All = 512
    };
    Q_DECLARE_FLAGS(States, State)

    /**
     * Constructs a new Action item
     *
     * \param actionId the PolicyKit action Id (e.g.: org.freedesktop.policykit.read)
     * \param parent the object parent
     */
    explicit Action(const QString &actionId = QString(), QObject *parent = 0);
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
     * \return \c true if the caller can do the action
     *
     * \see activated()
     */
    bool activate();

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
     * Returns the current action ID.
     *
     * \return The action ID
     *
     */
    QString actionId() const;

    /**
     * Sets the text for the current action. This will
     * be shown only in the states specified in the \c states parameter.
     * \param text the new text for the action
     * \param states the states of the Polkit action on which the setting
     *               will be applied
     */
    void setText(const QString &text, States states = All);

    /**
     * Sets the tooltip for the current action. This will
     * be shown only in the states specified in the \c states parameter.
     * \param toolTip the new tooltip for the action
     * \param states the states of the Polkit action on which the setting
     *               will be applied
     */
    void setToolTip(const QString &toolTip, States states = All);

    /**
     * Sets the whatsthis for the current action. This will
     * be shown only in the states specified in the \c states parameter.
     * \param whatsThis the new whatsthis for the action
     * \param states the states of the Polkit action on which the setting
     *               will be applied
     */
    void setWhatsThis(const QString &whatsThis, States states = All);

    /**
     * Sets the icon for the current action. This will
     * be shown only in the states specified in the \c states parameter.
     * \note You need to pass a QIcon here. You can easily
     *       create one from a Pixmap, or pass a KIcon
     * \param icon the new icon for the action
     * \param states the states of the Polkit action on which the setting
     *               will be applied
     */
    void setIcon(const QIcon &icon, States states = All);

    /**
     * Sets whether the current action is visible or not. This will
     * be applied only in the states specified in the \c states parameter.
     * \param text the new text for the action
     * \param states the states of the Polkit action on which the setting
     *               will be applied
     */
    void setVisible(bool visible, States states = All);

    /**
    * Sets whether the current action is enabled or not. This will
    * be shown only in the states specified in the \c states parameter.
    * \param text the new text for the action
    * \param states the states of the Polkit action on which the setting
    *               will be applied
    */
    void setEnabled(bool enabled, States states = All);

    /**
     * This function sets the process id of the target that
     * should receive the authorization. Set this to 0 to set
     * the current process as the target.
     *
     * \param pid The target process id; 0 if it is the current process
     */
    void setTargetPID(qint64 pid);

    /**
     * Gets the text of the action when it is in the specified state
     *
     * \note Passing None will return the current value
     * \param state The state to be checked
     * \returns The text shown when the action is in the specified state
     */
    QString text(State state = None) const;

    /**
     * Gets the tooltip of the action when it is in the specified state
     *
     * \note Passing None will return the current value
     * \param state The state to be checked
     * \returns The tooltip shown when the action is in the specified state
     */
    QString toolTip(State state = None) const;

    /**
     * Gets the whatsThis of the action when it is in the specified state
     *
     * \param state The state to be checked
     * \returns The whatsThis shown when the action is in the specified state
     */
    QString whatsThis(State state = None) const;

    /**
     * Gets the icon of the action when it is in the specified state
     *
     * \note Passing None will return the current value
     * \param state The state to be checked
     * \returns The icon shown when the action is in the specified state
     */
    QIcon icon(State state = None) const;

    /**
     * Gets whether the action is visible or not when it is in the specified state
     *
     * \note Passing None will return the current value
     * \param state The state to be checked
     * \returns Whether the action is visible or not in the specified state
     */
    bool isVisible(State state = None) const;

    /**
     * Gets whether the action is enabled or not when it is in the specified state
     *
     * \note Passing None will return the current value
     * \param state The state to be checked
     * \returns Whether the action is enabled or not in the specified state
     */
    bool isEnabled(State state = None) const;

    /**
     * \see setTargetPID
     */
    qint64 targetPID() const;

    /**
     * This method can be used to check the if the current action
     * can be performed (i.e. PolKitResult is YES).
     * \note This method does not call the authentication dialog, use
     *       activate() instead
     * \return \c true if the action can be performed
     */
    bool isAllowed() const;

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

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void configChanged())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PolkitQt::Action::States)

}

#endif
