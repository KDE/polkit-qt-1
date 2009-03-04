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
 */
class POLKIT_QT_EXPORT Action : public QObject
{
    Q_OBJECT
public:
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
     * \return Whether the Action should be visible
     */
    bool visible() const;

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
     * \return Whether the Action should be enabled
     */
    bool enabled() const;

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
     * \see setText
     */
    QString text() const;

    /**
     * The current Tool tip
     * \param toolTip set the toolTip in all four states
     */
    void setToolTip(const QString &toolTip);

    /**
     * \see setToolTip
     */
    QString toolTip() const;

    /**
     * The current What's this
     * \param whatsThis set the whatsThis in all four states
     */
    void setWhatsThis(const QString &whatsThis);

    /**
     * \see setWhatsThis
     */
    QString whatsThis() const;

    /**
     * The current Icon Name
     * \param icon set the icon in all four states
     */
    void setIcon(const QIcon &icon);

    /**
     * \see setIcon
     */
    QIcon icon() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, whether the
     * action will be visible
     */
    void setSelfBlockedVisible(bool value);

    /**
     * \see setSelfBlockedVisible
     */
    bool selfBlockedVisible() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, whether the
     * action will be enabled
     */
    void setSelfBlockedEnabled(bool value);

    /**
     * \see setSelfBlockedEnabled
     */
    bool selfBlockedEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this text
     */
    void setSelfBlockedText(const QString &text);

    /**
     * \see setSelfBlockedText
     */
    QString selfBlockedText() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this tooltip
     */
    void setSelfBlockedToolTip(const QString &toolTip);

    /**
     * \see setSelfBlockedToolTip
     */
    QString selfBlockedToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this short-text
     */
    void setSelfBlockedWhatsThis(const QString &whatsThis);

    /**
     * \see setSelfBlockedWhatsThis
     */
    QString selfBlockedWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this icon-name
     */
    void setSelfBlockedIcon(const QIcon &icon);

    /**
     * \see setSelfBlockedIcon
     */
    QIcon selfBlockedIcon() const;


    /**
     * If PolicyKit evaluates the result as 'no', whether the action will be visible
     */
    void setNoVisible(bool value);

    /**
     * \see setNoVisible
     */
    bool noVisible() const;

    /**
     * If PolicyKit evaluates the result as 'no', whether the action will be enabled
     * \note if you set this to TRUE the activated() signal will be
     * emitted even when the PolKitResult is POLKIT_RESULT_NO
     */
    void setNoEnabled(bool value);

    /**
     * \see setNoEnabled
     */
    bool noEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this text
     */
    void setNoText(const QString &text);

    /**
     * \see setNoText
     */
    QString noText() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this tooltip
     */
    void setNoToolTip(const QString &toolTip);

    /**
     * \see setNoToolTip
     */
    QString noToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this short-text
     */
    void setNoWhatsThis(const QString &whatsThis);

    /**
     * \see setNoWhatsThis
     */
    QString noWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this icon-name
     */
    void setNoIcon(const QIcon &icon);

    /**
     * \see setNoIcon
     */
    QIcon noIcon() const;


    /**
     * If PolicyKit evaluates the result as 'auth', whether the action will be visible
     */
    void setAuthVisible(bool value);

    /**
     * \see setAuthVisible
     */
    bool authVisible() const;

    /**
     * If PolicyKit evaluates the result as 'auth', whether the action will be enabled
     */
    void setAuthEnabled(bool value);

    /**
     * \see setAuthEnabled
     */
    bool authEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this text
     */
    void setAuthText(const QString &text);

    /**
     * \see setAuthText
     */
    QString authText() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this tooltip
     */
    void setAuthToolTip(const QString &toolTip);

    /**
     * \see setAuthToolTip
     */
    QString authToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this short-text
     */
    void setAuthWhatsThis(const QString &whatsThis);

    /**
     * \see setAuthWhatsThis
     */
    QString authWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this icon-name
     */
    void setAuthIcon(const QIcon &icon);

    /**
     * \see setAuthIcon
     */
    QIcon authIcon() const;


    /**
     * If PolicyKit evaluates the result as 'yes', whether the action will be visible
     */
    void setYesVisible(bool value);

    /**
     * \see setYesVisible
     */
    bool yesVisible() const;

    /**
     * If PolicyKit evaluates the result as 'yes', whether the action will be enabled
     */
    void setYesEnabled(bool value);

    /**
     * \see setYesEnabled
     */
    bool yesEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this text
     */
    void setYesText(const QString &text);

    /**
     * \see setYesText
     */
    QString yesText() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this tooltip
     */
    void setYesToolTip(const QString &toolTip);

     /**
     * \see setYesToolTip
     */
    QString yesToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this short-text
     */
    void setYesWhatsThis(const QString &whatsThis);

    /**
     * \see setYesWhatsThis
     */
    QString yesWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this icon-name
     */
    void setYesIcon(const QIcon &icon);

    /**
     * \see setYesIcon
     */
    QIcon yesIcon() const;

    /**
     * The target process id to receive the authorization; if 0 it is the current process
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
     * This method can be used to check the if the given action
     * can be performed (i.e. PolKitResult is YES).
     * \note This method does not call the authentication dialog, use
     *       activate() instead
     * \return \c true if the action can be performed
     */
    bool canDoAction() const;

private Q_SLOTS:
    void configChanged();

private:
    class Private;
    Private * const d;
};

}

#endif
