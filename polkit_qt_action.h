/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
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

#include <QtCore>

#include <QAction>
#include <QtCore/QString>

#include <polkit/polkit.h>

namespace PolKitQt {

/**
 * \class PolkitQtAuthProxy polkit_qt_auth.h PolkitQtAuthProxy
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to manage actions
 *
 * This class is an interface around the Polkit Actions
 * with this class you can track the policykit result
 * of a given action
 */
class PkAction : public QObject
{
Q_OBJECT
public:
    PkAction(const QString &actionId, WId winId, const QIcon &icon, const QString &text,  const QString &tooltip, QObject *parent);
    ~PkAction();

signals:
    /**
     * Emitted when PolicyKit configuration
     * (e.g. /etc/PolicyKit/PolicyKit.conf or .policy files)
     * changes content
     *
     * If you're not using this class as a proxy
     * connect to this signal to update your widgets
     */
    void resultChanged();

    /**
     * Emitted when using this class as a proxy
     * for a given action, it's only emmited after
     * activate() slot is called and the auth permits
     * the action
     */
    void activated();

public slots:
    /**
     * Use this slot if you want activated()
     * to be emitted if the given action is
     * autorized
     */
    void activate();

public:
    /**
     * Changes the action being tracked
     */
    void setPolkiAction(const QString &action_id);
    PolKitAction* polkiAction() const;

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
    void setSelfBlockedIconName(const QString &iconName);
    QString selfBlockedIconName() const;


    /**
     * If PolicyKit evaluates the result as 'no', whether the action will be visible
     */
    void setNoVisible(bool value);
    bool noVisible() const;

    /**
     * If PolicyKit evaluates the result as 'no', whether the action will be enabled
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
    void setNoIconName(const QString &iconName);
    QString noIconName() const;


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
    void setAuthIconName(const QString &iconName);
    QString authIconName() const;


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
    void setYesIconName(const QString &iconName);
    QString yesIconName() const;


    /**
     * Can be set to FALSE to force invisibility no matter what PolicyKit reports
     */
    void setMasterVisible(bool value);
    bool masterVisible() const;

    /**
     * Can be set to FALSE to force insensitivity no matter what PolicyKit reports
     */
    void setMasterEnabled(bool value);
    bool masterEnabled() const;


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
private slots:
    void configChanged();

private:
    QString       m_actionId;
    PolKitAction *m_pkAction;
    PolKitResult  m_pkResult;
    uint          m_targetPID;
    WId           m_winId;

    void         updateAction();
    bool         computePkResult();

    // current data
    bool    m_visible;
    bool    m_sensitive;
    QString m_text;
    QString m_whatsThis;
    QString m_tooltip;
    QString m_iconName;

    // states data
    bool    self_blocked_visible;
    bool    self_blocked_sensitive;
    QString self_blocked_text;
    QString self_blocked_short_text;
    QString self_blocked_tooltip;
    QString self_blocked_icon_name;

    bool    no_visible;
    bool    no_sensitive;
    QString no_text;
    QString no_short_text;
    QString no_tooltip;
    QString no_icon_name;

    bool    auth_visible;
    bool    auth_sensitive;
    QString auth_text;
    QString auth_short_text;
    QString auth_tooltip;
    QString auth_icon_name;

    bool    yes_visible;
    bool    yes_sensitive;
    QString yes_text;
    QString yes_short_text;
    QString yes_tooltip;
    QString yes_icon_name;

    bool master_visible;
    bool master_sensitive;
};

}

#endif
