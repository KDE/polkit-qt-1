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

#include <QtDBus/QtDBus>
#include <polkit-dbus/polkit-dbus.h>

using namespace PolKitQt;

QPkAction::QPkAction(const QString &actionId, WId winId, QObject *parent)
 : QObject(parent), m_pkAction(NULL), m_targetPID(0), m_winId(winId)
{
    // Set the default values
    m_selfBlockedVisible   = true;
    m_selfBlockedEnabled   = false;

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
    connect(QPkContext::instance(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
}

QPkAction::~QPkAction()
{
    if (m_pkAction != NULL)
        polkit_action_unref(m_pkAction);
}

bool QPkAction::activate()
{
    qDebug() << "QPkAction::activate()";
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
                if (QPkAuth::obtainAuth(m_actionId, m_winId, targetPID())) {
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

void QPkAction::updateAction()
{
    PolKitAuthorizationDB *authdb;
    if (QPkContext::instance()->hasError()) {
        return;
    }
    authdb = polkit_context_get_authorization_db (QPkContext::instance()->pkContext);

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

void QPkAction::configChanged()
{
    bool result_changed;
    result_changed = computePkResult();
    if (result_changed) {
        updateAction();
    }
}

bool QPkAction::computePkResult()
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

PolKitResult QPkAction::computePkResultDirect(PolKitAction *action, pid_t pid)
{
    qDebug() << "computePkResultDirect";

    PolKitCaller *pk_caller;
    PolKitResult pk_result;
    DBusError dbus_error;
    dbus_error_init (&dbus_error);

    if (QPkContext::instance()->hasError())
        return pk_result = POLKIT_RESULT_UNKNOWN;
    pk_caller = polkit_tracker_get_caller_from_pid (QPkContext::instance()->pkTracker,
                                                    pid,
                                                    &dbus_error);
    if (pk_caller == NULL) {
        qWarning("Cannot get PolKitCaller object for target (pid=%d): %s: %s",
                    pid, dbus_error.name, dbus_error.message);
        dbus_error_free (&dbus_error);

        /* this is bad so cop-out to UKNOWN */
        pk_result = POLKIT_RESULT_UNKNOWN;
    } else {
        pk_result = polkit_context_is_caller_authorized (QPkContext::instance()->pkContext,
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

pid_t QPkAction::targetPID()
{
    if (m_targetPID != 0)
        return m_targetPID;
    else
        return QCoreApplication::applicationPid();
}

void QPkAction::setTargetPID(pid_t pid)
{
    m_targetPID = pid;

    computePkResult();
    updateAction();
}

void QPkAction::setPolkitAction(const QString &actionId)
{
    qDebug() << "setPolkiAction" << actionId;
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

bool QPkAction::visible() const
{
    return m_visible;
}

bool QPkAction::enabled() const
{
    return m_enabled;
}

void QPkAction::setText(const QString &text)
{
    m_selfBlockedText = text;
    m_noText          = text;
    m_authText        = text;
    m_yesText         = text;
    updateAction();
}

QString QPkAction::text() const
{
    return m_text;
}

void QPkAction::setToolTip(const QString &toolTip)
{
    m_selfBlockedToolTip = toolTip;
    m_noToolTip          = toolTip;
    m_authToolTip        = toolTip;
    m_yesToolTip         = toolTip; 
    updateAction();
}

QString QPkAction::toolTip() const
{
    return m_toolTip;
}

void QPkAction::setWhatsThis(const QString &whatsThis)
{
    m_selfBlockedWhatsThis = whatsThis;
    m_noWhatsThis          = whatsThis;
    m_authWhatsThis        = whatsThis;
    m_yesWhatsThis         = whatsThis;
    updateAction();
}

QString QPkAction::whatsThis() const
{
    return m_whatsThis;
}

void QPkAction::setIcon(const QIcon &icon)
{
    m_selfBlockedIcon = icon;
    m_noIcon          = icon;
    m_authIcon        = icon;
    m_yesIcon         = icon;
    updateAction();
}

QIcon QPkAction::icon() const
{
    return m_icon;
}
//--------------------------------------------------
PolKitAction* QPkAction::polkitAction() const
{
    return m_pkAction;
}

QString QPkAction::actionId() const
{
    return m_actionId;
}
//---------------------------------------------------
void QPkAction::setSelfBlockedVisible(bool value)
{
    m_selfBlockedVisible = value;
    updateAction();
}

bool QPkAction::selfBlockedVisible() const
{
    return m_selfBlockedVisible;
}

void QPkAction::setSelfBlockedEnabled(bool value)
{
    m_selfBlockedEnabled = value;
    updateAction();
}

bool QPkAction::selfBlockedEnabled() const
{
    return m_selfBlockedEnabled;
}

void QPkAction::setSelfBlockedText(const QString &text)
{
    m_selfBlockedText = text;
    updateAction();
}

QString QPkAction::selfBlockedText() const
{
    return m_selfBlockedText;
}

void QPkAction::setSelfBlockedToolTip(const QString &toolTip)
{
    m_selfBlockedToolTip = toolTip;
    updateAction();
}

QString QPkAction::selfBlockedToolTip() const
{
    return m_selfBlockedToolTip;
}

void QPkAction::setSelfBlockedWhatsThis(const QString &whatsThis)
{
    m_selfBlockedWhatsThis = whatsThis;
    updateAction();
}

QString QPkAction::selfBlockedWhatsThis() const
{
    return m_selfBlockedWhatsThis;
}

void QPkAction::setSelfBlockedIcon(const QIcon &icon)
{
    m_selfBlockedIcon = icon;
    updateAction();
}

QIcon QPkAction::selfBlockedIcon() const
{
    return m_selfBlockedIcon;
}
//----------------------------------------------------------
void QPkAction::setNoVisible(bool value)
{
    m_noVisible = value;
    updateAction();
}

bool QPkAction::noVisible() const
{
    return m_noVisible;
}

void QPkAction::setNoEnabled(bool value)
{
    m_noEnabled = value;
    updateAction();
}

bool QPkAction::noEnabled() const
{
    return m_noEnabled;
}

void QPkAction::setNoText(const QString &text)
{
    m_noText = text;
    updateAction();
}

QString QPkAction::noText() const
{
    return m_noText;
}

void QPkAction::setNoToolTip(const QString &toolTip)
{
    m_noToolTip = toolTip;
    updateAction();
}

QString QPkAction::noToolTip() const
{
    return m_noToolTip;
}

void QPkAction::setNoWhatsThis(const QString &whatsThis)
{
    m_noWhatsThis = whatsThis;
    updateAction();
}

QString QPkAction::noWhatsThis() const
{
    return m_noWhatsThis;
}

void QPkAction::setNoIcon(const QIcon &icon)
{
    m_noIcon = icon;
    updateAction();
}

QIcon QPkAction::noIcon() const
{
    return m_noIcon;
}
//-----------------------------------------
void QPkAction::setAuthVisible(bool value)
{
    m_authVisible = value;
    updateAction();
}

bool QPkAction::authVisible() const
{
    return m_authVisible;
}

void QPkAction::setAuthEnabled(bool value)
{
    m_authEnabled = value;
    updateAction();
}

bool QPkAction::authEnabled() const
{
    return m_authEnabled;
}

void QPkAction::setAuthText(const QString &text)
{
    m_authText = text;
    updateAction();
}

QString QPkAction::authText() const
{
    return m_authText;
}

void QPkAction::setAuthToolTip(const QString &toolTip)
{
    m_authToolTip = toolTip;
    updateAction();
}

QString QPkAction::authToolTip() const
{
    return m_authToolTip;
}

void QPkAction::setAuthWhatsThis(const QString &whatsThis)
{
    m_authWhatsThis = whatsThis;
    updateAction();
}

QString QPkAction::authWhatsThis() const
{
    return m_authWhatsThis;
}

void QPkAction::setAuthIcon(const QIcon &icon)
{
    m_authIcon = icon;
    updateAction();
}

QIcon QPkAction::authIcon() const
{
    return m_authIcon;
}
//-------------------------------------------------
void QPkAction::setYesVisible(bool value)
{
    m_yesVisible = value;
    updateAction();
}

bool QPkAction::yesVisible() const
{
    return m_yesVisible;
}

void QPkAction::setYesEnabled(bool value)
{
    m_yesEnabled = value;
    updateAction();
}

bool QPkAction::yesEnabled() const
{
    return m_yesEnabled;
}

void QPkAction::setYesText(const QString &text)
{
    m_yesText = text;
    updateAction();
}

QString QPkAction::yesText() const
{
    return m_yesText;
}

void QPkAction::setYesToolTip(const QString &toolTip)
{
    m_yesToolTip = toolTip;
    updateAction();
}

QString QPkAction::yesToolTip() const
{
    return m_yesToolTip;
}

void QPkAction::setYesWhatsThis(const QString &whatsThis)
{
    m_yesWhatsThis = whatsThis;
    updateAction();
}

QString QPkAction::yesWhatsThis() const
{
    return m_yesWhatsThis;
}

void QPkAction::setYesIcon(const QIcon &icon)
{
    m_yesIcon = icon;
    updateAction();
}

QIcon QPkAction::yesIcon() const
{
    return m_yesIcon;
}
//------------------------------------------------------
void QPkAction::setMasterVisible(bool value)
{
    m_masterVisible = value;
    updateAction();
}

bool QPkAction::masterVisible() const
{
    return m_masterVisible;
}

void QPkAction::setMasterEnabled(bool value)
{
    m_masterEnabled = value;
    updateAction();
}

bool QPkAction::masterEnabled() const
{
    return m_masterEnabled;
}

#include "polkit_qt_action.moc"
