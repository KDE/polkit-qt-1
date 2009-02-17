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

namespace QPolicyKit
{

/**
 * \class Action polkit_qt_auth.h Action
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to manage actions
 *
 * This class is an interface around the Polkit Actions
 * with this class you can track the policykit result
 * of a given action
 */
class POLKIT_QT_EXPORT Action : public QObject
{
    Q_OBJECT
public:
    Action(const QString &actionId, QObject *parent);
    ~Action();

Q_SIGNALS:
    /**
     * Emitted when PolicyKit the PolKitResult for
     * the given action or the internal data changes
     * (ie the user called one of the set methods)
     * You should connect to this signal if you want
     * to track these changes.
     */
    void dataChanged();

    /**
     * Emitted when using this class as a proxy
     * for a given action, it's only emmited after
     * activate() slot is called and the auth permits
     * the action
     */
    void activated();

public Q_SLOTS:
    /**
     * Use this slot if you want activated()
     * to be emitted if the given action is
     * autorized
     *
     * \note this will call the auth dialog
     * if needed. Only use the return value if
     * you want a sync behavior
     *
     * \return TRUE if the caller can do the action
     */
    bool activate(WId winId = 0);

public:
    /**
     * Changes the action being tracked
     */
    void setPolkitAction(const QString &actionId);
    PolKitAction* polkitAction() const;
    QString       actionId() const;

    /**
     * Whether the Action should be visible
     */
    bool visible() const;

    /**
     * Can be set to FALSE to force invisibility no matter what PolicyKit reports
     */
    void setMasterVisible(bool value);
    bool masterVisible() const;

    /**
     * Whether the Action should be enabled
     */
    bool enabled() const;

    /**
     * Can be set to FALSE to force disabled no matter what PolicyKit reports
     */
    void setMasterEnabled(bool value);
    bool masterEnabled() const;

    /**
     * The current Text for the given action
     * \p text set the text in all four states
     */
    void setText(const QString &text);
    QString text() const;

    /**
     * The current Tool tip
     * \p toolTip set the toolTip in all four states
     */
    void setToolTip(const QString &toolTip);
    QString toolTip() const;

    /**
     * The current What's this
     * \p whatsThis set the whatsThis in all four states
     */
    void setWhatsThis(const QString &whatsThis);
    QString whatsThis() const;

    /**
     * The current Icon Name
     * \p icon set the icon in all four states
     */
    void setIcon(const QIcon &icon);
    QIcon icon() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, whether the
     * action will be visible
     */
    void setSelfBlockedVisible(bool value);
    bool selfBlockedVisible() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, whether the
     * action will be enabled
     */
    void setSelfBlockedEnabled(bool value);
    bool selfBlockedEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this text
     */
    void setSelfBlockedText(const QString &text);
    QString selfBlockedText() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this tooltip
     */
    void setSelfBlockedToolTip(const QString &toolTip);
    QString selfBlockedToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this short-text
     */
    void setSelfBlockedWhatsThis(const QString &whatsThis);
    QString selfBlockedWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'no' and the reason is that
     * the user has a self-granted negative authorization, use this icon-name
     */
    void setSelfBlockedIcon(const QIcon &icon);
    QIcon selfBlockedIcon() const;


    /**
     * If PolicyKit evaluates the result as 'no', whether the action will be visible
     */
    void setNoVisible(bool value);
    bool noVisible() const;

    /**
     * If PolicyKit evaluates the result as 'no', whether the action will be enabled
     * \note if you set this to TRUE the activated() signal will be
     * emitted even when the PolKitResult is POLKIT_RESULT_NO
     */
    void setNoEnabled(bool value);
    bool noEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this text
     */
    void setNoText(const QString &text);
    QString noText() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this tooltip
     */
    void setNoToolTip(const QString &toolTip);
    QString noToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this short-text
     */
    void setNoWhatsThis(const QString &whatsThis);
    QString noWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'no', use this icon-name
     */
    void setNoIcon(const QIcon &icon);
    QIcon noIcon() const;


    /**
     * If PolicyKit evaluates the result as 'auth', whether the action will be visible
     */
    void setAuthVisible(bool value);
    bool authVisible() const;

    /**
     * If PolicyKit evaluates the result as 'auth', whether the action will be enabled
     */
    void setAuthEnabled(bool value);
    bool authEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this text
     */
    void setAuthText(const QString &text);
    QString authText() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this tooltip
     */
    void setAuthToolTip(const QString &toolTip);
    QString authToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this short-text
     */
    void setAuthWhatsThis(const QString &whatsThis);
    QString authWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'auth', use this icon-name
     */
    void setAuthIcon(const QIcon &icon);
    QIcon authIcon() const;


    /**
     * If PolicyKit evaluates the result as 'yes', whether the action will be visible
     */
    void setYesVisible(bool value);
    bool yesVisible() const;

    /**
     * If PolicyKit evaluates the result as 'yes', whether the action will be enabled
     */
    void setYesEnabled(bool value);
    bool yesEnabled() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this text
     */
    void setYesText(const QString &text);
    QString yesText() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this tooltip
     */
    void setYesToolTip(const QString &toolTip);
    QString yesToolTip() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this short-text
     */
    void setYesWhatsThis(const QString &whatsThis);
    QString yesWhatsThis() const;

    /**
     * If PolicyKit evaluates the result as 'yes', use this icon-name
     */
    void setYesIcon(const QIcon &icon);
    QIcon yesIcon() const;

    /**
     * The target process id to receive the authorization; if 0 it is the current process
     */
    void  setTargetPID(pid_t pid);
    pid_t targetPID();

    /**
     * This method can be used to compute the Polkit
     * Result of a given action
     * \p action the PolKitAction in question
     * \p pid the process id that the action can be requested for
     */
    static PolKitResult computePkResultDirect(PolKitAction *action, pid_t pid);

    /**
     * This method to check the if the given action can be performed
     * ie. PolKitResult is YES.
     * \note This method does not call the authentication dialog
     * \return TRUE if the action can be done
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
