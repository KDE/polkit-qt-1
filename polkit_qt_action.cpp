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

#include "polkit_qt_action.h"
#include "polkit_qt_context.h"
#include "polkit_qt_auth.h"

#include <QtCore/QDebug>

#include <polkit-dbus/polkit-dbus.h>

using namespace QPolicyKit;

Action::Action(const QString &actionId, QObject *parent)
 : QObject(parent), m_pkAction(NULL), m_targetPID(0)
{
    // Set the default values
    m_selfBlockedVisible = true;
    m_selfBlockedEnabled = false;

    m_noVisible     = true;
    m_noEnabled     = false;

    m_authVisible   = true;
    m_authEnabled   = true;

    m_yesVisible    = true;
    m_yesEnabled    = true;

    m_masterVisible = true;
    m_masterEnabled = true;

    // this must be called AFTER the values initialization
    setPolkitAction(actionId);

    // track the config changes to update the action
    connect(Context::instance(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
}

Action::~Action()
{
    if (m_pkAction != NULL)
        polkit_action_unref(m_pkAction);
}

bool Action::activate(WId winId)
{
    qDebug() << "Action::activate()";
    switch (m_pkResult) {
        case POLKIT_RESULT_YES:
            // If PolicyKit says yes.. emit the 'activated' signal
            emit activated();
            return true;
            break;

        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_SESSION:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_ALWAYS:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_SESSION:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_ALWAYS:
            /* Otherwise, if the action needs auth..  stop the emission
             * and start auth process..
             */
            if (m_pkAction != NULL) {
                if (Auth::obtainAuth(m_actionId, winId, targetPID())) {
                    // Make sure our result is up to date
                    computePkResult();
                    // emit activated as the obtain auth said it was ok
                    emit activated();
                    return true;
                }
            }
            break;

        default:
        case POLKIT_RESULT_NO:
            if (m_noEnabled) {
                /* If PolicyKit says no... and we got here.. it means
                 * that the user set the property "no-enabled" to
                 * TRUE..
                 *
                 * Hence, they probably have a good reason for doing
                 * this so do let the 'activate' signal propagate..
                 */
                emit activated();
                return true;
            }
            break;
    }
    return false;
}

void Action::updateAction()
{
    PolKitAuthorizationDB *authdb;
    if (Context::instance()->hasError()) {
        return;
    }
    authdb = polkit_context_get_authorization_db (Context::instance()->pkContext);

    switch (m_pkResult) {
        default:
        case POLKIT_RESULT_UNKNOWN:
        case POLKIT_RESULT_NO:
            /* TODO: see if we're self-blocked */
            if (m_pkAction != NULL &&
                polkit_authorization_db_is_uid_blocked_by_self (authdb,
                                                                m_pkAction,
                                                                getuid (),
                                                                NULL)) {
                m_visible   = m_selfBlockedVisible && m_masterVisible;
                m_enabled   = m_selfBlockedEnabled && m_masterEnabled;
                m_whatsThis = m_selfBlockedWhatsThis;
                m_text      = m_selfBlockedText;
                m_toolTip   = m_selfBlockedToolTip;
                m_icon      = m_selfBlockedIcon;
            } else {
                m_visible   = m_noVisible && m_masterVisible;
                m_enabled   = m_noEnabled && m_masterEnabled;
                m_whatsThis = m_noWhatsThis;
                m_text      = m_noText;
                m_toolTip   = m_noToolTip;
                m_icon      = m_noIcon;
            }
            break;

        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_SESSION:
        case POLKIT_RESULT_ONLY_VIA_ADMIN_AUTH_KEEP_ALWAYS:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_ONE_SHOT:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_SESSION:
        case POLKIT_RESULT_ONLY_VIA_SELF_AUTH_KEEP_ALWAYS:
            m_visible   = m_authVisible && m_masterVisible;
            m_enabled   = m_authEnabled && m_masterEnabled;
            m_whatsThis = m_authWhatsThis;
            m_text      = m_authText;
            m_toolTip   = m_authToolTip;
            m_icon      = m_authIcon;
            break;

        case POLKIT_RESULT_YES:
            m_visible   = m_yesVisible && m_masterVisible;
            m_enabled   = m_yesEnabled && m_masterEnabled;
            m_whatsThis = m_yesWhatsThis;
            m_text      = m_yesText;
            m_toolTip   = m_yesToolTip;
            m_icon      = m_yesIcon;
            break;
    }
    emit dataChanged();
}

void Action::configChanged()
{
    bool result_changed;
    result_changed = computePkResult();
    if (result_changed) {
        updateAction();
    }
}

bool Action::computePkResult()
{
    PolKitResult old_result;

    old_result = m_pkResult;
    m_pkResult = POLKIT_RESULT_UNKNOWN;

    if (m_pkAction == NULL) {
        m_pkResult = POLKIT_RESULT_YES;
    } else {
        m_pkResult = computePkResultDirect(m_pkAction, targetPID());
    }

    return old_result != m_pkResult;
}

PolKitResult Action::computePkResultDirect(PolKitAction *action, pid_t pid)
{
    qDebug() << "computePkResultDirect";

    PolKitCaller *pk_caller;
    PolKitResult pk_result;
    DBusError dbus_error;
    dbus_error_init (&dbus_error);

    if (Context::instance()->hasError())
        return pk_result = POLKIT_RESULT_UNKNOWN;
    pk_caller = polkit_tracker_get_caller_from_pid (Context::instance()->pkTracker,
                                                    pid,
                                                    &dbus_error);
    if (pk_caller == NULL) {
        qWarning("Cannot get PolKitCaller object for target (pid=%d): %s: %s",
                    pid, dbus_error.name, dbus_error.message);
        dbus_error_free (&dbus_error);

        /* this is bad so cop-out to UKNOWN */
        pk_result = POLKIT_RESULT_UNKNOWN;
    } else {
        pk_result = polkit_context_is_caller_authorized (Context::instance()->pkContext,
                                                            action,
                                                            pk_caller,
                                                            FALSE,
                                                            NULL);
//         TODO try to find out when this would be used.. :(
//         if (pk_result != POLKIT_RESULT_YES) {
//             GSList *i;
//
//             /* no dice.. see if one if the sufficient actions, if any, yields a YES */
//             for (i = action->priv->polkit_action_sufficient; i != NULL; i = g_slist_next (i)) {
//                 PolKitResult r;
//                 PolKitAction *a = (PolKitAction *) i->data;
//
//                 r = polkit_context_is_caller_authorized (action->priv->pk_g_context->pk_context,
//                                                             a,
//                                                             pk_caller,
//                                                             FALSE,
//                                                             NULL);
//
//                 if (r == POLKIT_RESULT_YES) {
//                     pk_result = r;
//                     break;
//                 }
//             }
//         }
    }

    if (pk_caller != NULL)
            polkit_caller_unref (pk_caller);

    return pk_result;
}

pid_t Action::targetPID()
{
    if (m_targetPID != 0)
        return m_targetPID;
    else
        return QCoreApplication::applicationPid();
}

void Action::setTargetPID(pid_t pid)
{
    m_targetPID = pid;

    computePkResult();
    updateAction();
}

void Action::setPolkitAction(const QString &actionId)
{
    qDebug() << "setPolkitAction" << actionId;
    PolKitAction *pkAction = polkit_action_new();
    polkit_action_set_action_id(pkAction, actionId.toAscii().data());
    /* Don't bother updating polkit_action if it's the same
     * value.. it will just cause a lot of unnecessary work as
     * we'll recompute the answer via PolicyKit..
     *
     * unless it's on the initial call (where priv->polkit_action
     * is alread NULL) because we need that initial update;
     */
    if (!m_pkAction || m_pkAction != pkAction) {
//         action->priv->polkit_action_set_once = TRUE;
        if (m_pkAction != NULL)
                polkit_action_unref (m_pkAction);
        if (pkAction != NULL) {
            m_pkAction = polkit_action_ref (pkAction);
            m_actionId = actionId;
        } else {
            m_pkAction = NULL;
            m_actionId.clear();
        }
        computePkResult();
        updateAction();
    }
}

bool Action::canDoAction() const
{
    return m_pkResult == POLKIT_RESULT_YES;
}

bool Action::visible() const
{
    return m_visible;
}

bool Action::enabled() const
{
    return m_enabled;
}

void Action::setText(const QString &text)
{
    m_selfBlockedText = text;
    m_noText          = text;
    m_authText        = text;
    m_yesText         = text;
    updateAction();
}

QString Action::text() const
{
    return m_text;
}

void Action::setToolTip(const QString &toolTip)
{
    m_selfBlockedToolTip = toolTip;
    m_noToolTip          = toolTip;
    m_authToolTip        = toolTip;
    m_yesToolTip         = toolTip;
    updateAction();
}

QString Action::toolTip() const
{
    return m_toolTip;
}

void Action::setWhatsThis(const QString &whatsThis)
{
    m_selfBlockedWhatsThis = whatsThis;
    m_noWhatsThis          = whatsThis;
    m_authWhatsThis        = whatsThis;
    m_yesWhatsThis         = whatsThis;
    updateAction();
}

QString Action::whatsThis() const
{
    return m_whatsThis;
}

void Action::setIcon(const QIcon &icon)
{
    m_selfBlockedIcon = icon;
    m_noIcon          = icon;
    m_authIcon        = icon;
    m_yesIcon         = icon;
    updateAction();
}

QIcon Action::icon() const
{
    return m_icon;
}
//--------------------------------------------------
PolKitAction* Action::polkitAction() const
{
    return m_pkAction;
}

QString Action::actionId() const
{
    return m_actionId;
}
//---------------------------------------------------
void Action::setSelfBlockedVisible(bool value)
{
    m_selfBlockedVisible = value;
    updateAction();
}

bool Action::selfBlockedVisible() const
{
    return m_selfBlockedVisible;
}

void Action::setSelfBlockedEnabled(bool value)
{
    m_selfBlockedEnabled = value;
    updateAction();
}

bool Action::selfBlockedEnabled() const
{
    return m_selfBlockedEnabled;
}

void Action::setSelfBlockedText(const QString &text)
{
    m_selfBlockedText = text;
    updateAction();
}

QString Action::selfBlockedText() const
{
    return m_selfBlockedText;
}

void Action::setSelfBlockedToolTip(const QString &toolTip)
{
    m_selfBlockedToolTip = toolTip;
    updateAction();
}

QString Action::selfBlockedToolTip() const
{
    return m_selfBlockedToolTip;
}

void Action::setSelfBlockedWhatsThis(const QString &whatsThis)
{
    m_selfBlockedWhatsThis = whatsThis;
    updateAction();
}

QString Action::selfBlockedWhatsThis() const
{
    return m_selfBlockedWhatsThis;
}

void Action::setSelfBlockedIcon(const QIcon &icon)
{
    m_selfBlockedIcon = icon;
    updateAction();
}

QIcon Action::selfBlockedIcon() const
{
    return m_selfBlockedIcon;
}
//----------------------------------------------------------
void Action::setNoVisible(bool value)
{
    m_noVisible = value;
    updateAction();
}

bool Action::noVisible() const
{
    return m_noVisible;
}

void Action::setNoEnabled(bool value)
{
    m_noEnabled = value;
    updateAction();
}

bool Action::noEnabled() const
{
    return m_noEnabled;
}

void Action::setNoText(const QString &text)
{
    m_noText = text;
    updateAction();
}

QString Action::noText() const
{
    return m_noText;
}

void Action::setNoToolTip(const QString &toolTip)
{
    m_noToolTip = toolTip;
    updateAction();
}

QString Action::noToolTip() const
{
    return m_noToolTip;
}

void Action::setNoWhatsThis(const QString &whatsThis)
{
    m_noWhatsThis = whatsThis;
    updateAction();
}

QString Action::noWhatsThis() const
{
    return m_noWhatsThis;
}

void Action::setNoIcon(const QIcon &icon)
{
    m_noIcon = icon;
    updateAction();
}

QIcon Action::noIcon() const
{
    return m_noIcon;
}
//-----------------------------------------
void Action::setAuthVisible(bool value)
{
    m_authVisible = value;
    updateAction();
}

bool Action::authVisible() const
{
    return m_authVisible;
}

void Action::setAuthEnabled(bool value)
{
    m_authEnabled = value;
    updateAction();
}

bool Action::authEnabled() const
{
    return m_authEnabled;
}

void Action::setAuthText(const QString &text)
{
    m_authText = text;
    updateAction();
}

QString Action::authText() const
{
    return m_authText;
}

void Action::setAuthToolTip(const QString &toolTip)
{
    m_authToolTip = toolTip;
    updateAction();
}

QString Action::authToolTip() const
{
    return m_authToolTip;
}

void Action::setAuthWhatsThis(const QString &whatsThis)
{
    m_authWhatsThis = whatsThis;
    updateAction();
}

QString Action::authWhatsThis() const
{
    return m_authWhatsThis;
}

void Action::setAuthIcon(const QIcon &icon)
{
    m_authIcon = icon;
    updateAction();
}

QIcon Action::authIcon() const
{
    return m_authIcon;
}
//-------------------------------------------------
void Action::setYesVisible(bool value)
{
    m_yesVisible = value;
    updateAction();
}

bool Action::yesVisible() const
{
    return m_yesVisible;
}

void Action::setYesEnabled(bool value)
{
    m_yesEnabled = value;
    updateAction();
}

bool Action::yesEnabled() const
{
    return m_yesEnabled;
}

void Action::setYesText(const QString &text)
{
    m_yesText = text;
    updateAction();
}

QString Action::yesText() const
{
    return m_yesText;
}

void Action::setYesToolTip(const QString &toolTip)
{
    m_yesToolTip = toolTip;
    updateAction();
}

QString Action::yesToolTip() const
{
    return m_yesToolTip;
}

void Action::setYesWhatsThis(const QString &whatsThis)
{
    m_yesWhatsThis = whatsThis;
    updateAction();
}

QString Action::yesWhatsThis() const
{
    return m_yesWhatsThis;
}

void Action::setYesIcon(const QIcon &icon)
{
    m_yesIcon = icon;
    updateAction();
}

QIcon Action::yesIcon() const
{
    return m_yesIcon;
}
//------------------------------------------------------
void Action::setMasterVisible(bool value)
{
    m_masterVisible = value;
    updateAction();
}

bool Action::masterVisible() const
{
    return m_masterVisible;
}

void Action::setMasterEnabled(bool value)
{
    m_masterEnabled = value;
    updateAction();
}

bool Action::masterEnabled() const
{
    return m_masterEnabled;
}

#include "polkit_qt_action.moc"
