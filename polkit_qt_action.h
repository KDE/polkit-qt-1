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

#include "polkit_qt_export.h"

#include <QtCore>

#include <QAction>
#include <QtCore/QString>

#include <polkit/polkit.h>

namespace PolKitQt {

/**
 * \class PkAction polkit_qt_auth.h PkAction
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to manage actions
 *
 * This class is an interface around the Polkit Actions
 * with this class you can track the policykit result
 * of a given action
 */
class POLKIT_QT_EXPORT PkAction : public QObject
{
Q_OBJECT
public:
    PkAction(const QString &actionId, WId winId, const QIcon &icon, const QString &text,  const QString &tooltip, QObject *parent);
    ~PkAction();

signals:
    /**
     * Emitted when PolicyKit the PolKitResul for
     * the given action changes, you should connect
     * to this signal if you want to track these
     * changes.
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
    void setPolkiAction(const QString &actionId);
    PolKitAction* polkiAction() const;
    QString       actionId() const;

    /**
     * Whether the Action should be visible
     */
//     void setVisible(bool value);
    bool visible() const;

    /**
     * Whether the Action should be enabled
     */
//     void setEnabled(bool value);
    bool enabled() const;

    /**
     * The current Text for the given action
     */
//     void setText(const QString &text);
    QString text() const;

    /**
     * The current Tool tip
     */
//     void setToolTip(const QString &toolTip);
    QString toolTip() const;

    /**
     * The current What's this
     */
//     void setWhatsThis(const QString &whatsThis);
    QString whatsThis() const;

    /**
     * The current Icon Name
     */
//     void setIcon(const QString &icon);
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
    bool    m_enabled;
    QString m_text;
    QString m_whatsThis;
    QString m_tooltip;
    QIcon   m_icon;

    // states data
    bool    m_selfBlockedVisible;
    bool    m_selfBlockedEnabled;
    QString m_selfBlockedText;
    QString m_selfBlockedWhatsThis;
    QString m_selfBlockedToolTip;
    QIcon   m_selfBlockedIcon;

    bool    m_noVisible;
    bool    m_noEnabled;
    QString m_noText;
    QString m_noWhatsThis;
    QString m_noToolTip;
    QIcon   m_noIcon;

    bool    m_authVisible;
    bool    m_authEnabled;
    QString m_authText;
    QString m_authWhatsThis;
    QString m_authToolTip;
    QIcon   m_authIcon;

    bool    m_yesVisible;
    bool    m_yesEnabled;
    QString m_yesText;
    QString m_yesWhatsThis;
    QString m_yesToolTip;
    QIcon   m_yesIcon;

    bool m_masterVisible;
    bool m_masterEnabled;
};

}

#endif
