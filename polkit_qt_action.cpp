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

PkAction::PkAction(const QString &actionId, WId winId, const QIcon &icon, const QString &text,  const QString &tooltip, QObject *parent)
 : QObject(parent), m_pkAction(0), m_targetPID(0), m_winId(winId)
{
    // Set the default values
    m_selfBlockedVisible   = true;
    m_selfBlockedEnabled   = false;
    m_selfBlockedText      = text;
    m_selfBlockedWhatsThis = tooltip;
    m_selfBlockedToolTip   = tooltip;
    m_selfBlockedIcon      = icon;

    m_noVisible     = true;
    m_noEnabled     = false;
    m_noText        = text;
    m_noWhatsThis   = tooltip;
    m_noToolTip     = tooltip;
    m_noIcon        = icon;

    m_authVisible   = true;
    m_authEnabled   = true;
    m_authText      = text;
    m_authWhatsThis = tooltip;
    m_authToolTip   = tooltip;
    m_authIcon      = icon;

    m_yesVisible    = true;
    m_yesEnabled    = true;
    m_yesText       = text;
    m_yesWhatsThis  = tooltip;
    m_yesToolTip    = tooltip;
    m_yesIcon       = icon;

    m_masterVisible = true;
    m_masterEnabled = true;

    // this must be called after the values initialization
    setPolkiAction(actionId);

    // track the config changes to update the action
    connect(PkContext::instance(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
}

PkAction::~PkAction()
{
    polkit_action_unref (m_pkAction);
}

void PkAction::activate()
{
    qDebug() << "PkAction::activate()";
    switch (m_pkResult) {
        case POLKIT_RESULT_YES:
            // If PolicyKit says yes.. emit the 'activated' signal
            emit activated();
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

//                 g_signal_stop_emission_by_name (action, "activate");

            if (m_pkAction != 0) {
//                         g_signal_emit (action, signals [AUTH_START_SIGNAL], 0);
                PkAuth::obtainAuth(m_actionId, m_winId, targetPID());
            }
            break;

        default:
        case POLKIT_RESULT_NO:
            /* If PolicyKit says no... and we got here.. it means
             * that the user set the property "no-sensitive" to
             * TRUE.. Otherwise we couldn't be handling this signal.
             *
             * Hence, they probably have a good reason for doing
             * this so do let the 'activate' signal propagate..
             */
            // TODO not sure in Qt we should do this
            emit activated();
            break;
    }
}

void PkAction::updateAction()
{
        PolKitAuthorizationDB *authdb;
        if (PkContext::instance()->hasError())
            return;
        authdb = polkit_context_get_authorization_db (PkContext::instance()->pkContext);

        switch (m_pkResult) {
            default:
            case POLKIT_RESULT_UNKNOWN:
            case POLKIT_RESULT_NO:
                /* TODO: see if we're self-blocked */
                if (m_pkAction != 0 &&
                    polkit_authorization_db_is_uid_blocked_by_self (authdb,
                                                                    m_pkAction,
                                                                    getuid (),
                                                                    NULL)) {
                    m_visible   = m_selfBlockedVisible && m_masterVisible;
                    m_enabled   = m_selfBlockedEnabled && m_masterEnabled;
                    m_whatsThis = m_selfBlockedWhatsThis;
                    m_text      = m_selfBlockedText;
                    m_tooltip   = m_selfBlockedToolTip;
                    m_icon      = m_selfBlockedIcon;
                } else {
                    m_visible   = m_noVisible && m_masterVisible;
                    m_enabled   = m_noEnabled && m_masterEnabled;
                    m_whatsThis = m_noWhatsThis;
                    m_text      = m_noText;
                    m_tooltip   = m_noToolTip;
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
                m_tooltip   = m_authToolTip;
                m_icon      = m_authIcon;
                break;

            case POLKIT_RESULT_YES:
                m_visible   = m_yesVisible && m_masterVisible;
                m_enabled   = m_yesEnabled && m_masterEnabled;
                m_whatsThis = m_yesWhatsThis;
                m_text      = m_yesText;
                m_tooltip   = m_yesToolTip;
                m_icon      = m_yesIcon;
                break;
        }
}

void PkAction::configChanged()
{
    bool result_changed;
    result_changed = computePkResult();
    updateAction();
    if (result_changed) {
        emit resultChanged();
    }
}

bool PkAction::computePkResult()
{
    PolKitResult old_result;

    old_result = m_pkResult;
    m_pkResult = POLKIT_RESULT_UNKNOWN;

    if (m_pkAction == 0) {
        m_pkResult = POLKIT_RESULT_YES;
    } else {
        m_pkResult = computePkResultDirect(m_pkAction, targetPID());
    }

    return old_result != m_pkResult;
}

PolKitResult PkAction::computePkResultDirect(PolKitAction *action, pid_t pid)
{
    qDebug() << "computePkResultDirect";

    PolKitCaller *pk_caller;
    PolKitResult pk_result;
    DBusError dbus_error;
    dbus_error_init (&dbus_error);

    if (PkContext::instance()->hasError())
        return pk_result = POLKIT_RESULT_UNKNOWN;
    pk_caller = polkit_tracker_get_caller_from_pid (PkContext::instance()->pkTracker,
                                                    pid,
                                                    &dbus_error);
    if (pk_caller == NULL) {
        qWarning("Cannot get PolKitCaller object for target (pid=%d): %s: %s",
                    pid, dbus_error.name, dbus_error.message);
        dbus_error_free (&dbus_error);

        /* this is bad so cop-out to UKNOWN */
        pk_result = POLKIT_RESULT_UNKNOWN;
    } else {
        pk_result = polkit_context_is_caller_authorized (PkContext::instance()->pkContext,
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

pid_t PkAction::targetPID()
{
    if (m_targetPID != 0)
        return m_targetPID;
    else
        return QCoreApplication::applicationPid();
}

void PkAction::setTargetPID(pid_t pid)
{
    m_targetPID = pid;

    computePkResult();
    updateAction();
}

void PkAction::setPolkiAction(const QString &actionId)
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
        if (m_pkAction != 0)
                polkit_action_unref (m_pkAction);
        if (pkAction != NULL) {
            m_pkAction = polkit_action_ref (pkAction);
            m_actionId = actionId;
        } else {
            m_pkAction = 0;
            m_actionId.clear();
        }
        computePkResult();
        updateAction();
    }
}

PolKitAction* PkAction::polkiAction() const
{
    return m_pkAction;
}

QString PkAction::actionId() const
{
    return m_actionId;
}

void PkAction::setSelfBlockedVisible(bool value)
{
    m_selfBlockedVisible = value;
    updateAction();
}

bool PkAction::selfBlockedVisible() const
{
    return m_selfBlockedVisible;
}

void PkAction::setSelfBlockedEnabled(bool value)
{
    m_selfBlockedEnabled = value;
    updateAction();
}

bool PkAction::selfBlockedEnabled() const
{
    return m_selfBlockedEnabled;
}

void PkAction::setSelfBlockedText(const QString &text)
{
    m_selfBlockedText = text;
    updateAction();
}

QString PkAction::selfBlockedText() const
{
    return m_selfBlockedText;
}

void PkAction::setSelfBlockedToolTip(const QString &toolTip)
{
    m_selfBlockedToolTip = toolTip;
    updateAction();
}

QString PkAction::selfBlockedToolTip() const
{
    return m_selfBlockedToolTip;
}

void PkAction::setSelfBlockedWhatsThis(const QString &whatsThis)
{
    m_selfBlockedWhatsThis = whatsThis;
    updateAction();
}

QString PkAction::selfBlockedWhatsThis() const
{
    return m_selfBlockedWhatsThis;
}

void PkAction::setSelfBlockedIcon(const QIcon &icon)
{
    m_selfBlockedIcon = icon;
    updateAction();
}

QIcon PkAction::selfBlockedIcon() const
{
    return m_selfBlockedIcon;
}
//----------------------------------------------------------
void PkAction::setNoVisible(bool value)
{
    m_noVisible = value;
    updateAction();
}

bool PkAction::noVisible() const
{
    return m_noVisible;
}

void PkAction::setNoEnabled(bool value)
{
    m_noEnabled = value;
    updateAction();
}

bool PkAction::noEnabled() const
{
    return m_noEnabled;
}

void PkAction::setNoText(const QString &text)
{
    m_noText = text;
    updateAction();
}

QString PkAction::noText() const
{
    return m_noText;
}

void PkAction::setNoToolTip(const QString &toolTip)
{
    m_noToolTip = toolTip;
    updateAction();
}

QString PkAction::noToolTip() const
{
    return m_noToolTip;
}

void PkAction::setNoWhatsThis(const QString &whatsThis)
{
    m_noWhatsThis = whatsThis;
    updateAction();
}

QString PkAction::noWhatsThis() const
{
    return m_noWhatsThis;
}

void PkAction::setNoIcon(const QIcon &icon)
{
    m_noIcon = icon;
    updateAction();
}

QIcon PkAction::noIcon() const
{
    return m_noIcon;
}
//-----------------------------------------
void PkAction::setAuthVisible(bool value)
{
    m_authVisible = value;
    updateAction();
}

bool PkAction::authVisible() const
{
    return m_authVisible;
}

void PkAction::setAuthEnabled(bool value)
{
    m_authEnabled = value;
    updateAction();
}

bool PkAction::authEnabled() const
{
    return m_authEnabled;
}

void PkAction::setAuthText(const QString &text)
{
    m_authText = text;
    updateAction();
}

QString PkAction::authText() const
{
    return m_authText;
}

void PkAction::setAuthToolTip(const QString &toolTip)
{
    m_authToolTip = toolTip;
    updateAction();
}

QString PkAction::authToolTip() const
{
    return m_authToolTip;
}

void PkAction::setAuthWhatsThis(const QString &whatsThis)
{
    m_authWhatsThis = whatsThis;
    updateAction();
}

QString PkAction::authWhatsThis() const
{
    return m_authWhatsThis;
}

void PkAction::setAuthIcon(const QIcon &icon)
{
    m_authIcon = icon;
    updateAction();
}

QIcon PkAction::authIcon() const
{
    return m_authIcon;
}
//-------------------------------------------------
void PkAction::setYesVisible(bool value)
{
    m_yesVisible = value;
    updateAction();
}

bool PkAction::yesVisible() const
{
    return m_yesVisible;
}

void PkAction::setYesEnabled(bool value)
{
    m_yesEnabled = value;
    updateAction();
}

bool PkAction::yesEnabled() const
{
    return m_yesEnabled;
}

void PkAction::setYesText(const QString &text)
{
    m_yesText = text;
    updateAction();
}

QString PkAction::yesText() const
{
    return m_yesText;
}

void PkAction::setYesToolTip(const QString &toolTip)
{
    m_yesToolTip = toolTip;
    updateAction();
}

QString PkAction::yesToolTip() const
{
    return m_yesToolTip;
}

void PkAction::setYesWhatsThis(const QString &whatsThis)
{
    m_yesWhatsThis = whatsThis;
    updateAction();
}

QString PkAction::yesWhatsThis() const
{
    return m_yesWhatsThis;
}

void PkAction::setYesIcon(const QIcon &icon)
{
    m_yesIcon = icon;
    updateAction();
}

QIcon PkAction::yesIcon() const
{
    return m_yesIcon;
}
//------------------------------------------------------
void PkAction::setMasterVisible(bool value)
{
    m_masterVisible = value;
    updateAction();
}

bool PkAction::masterVisible() const
{
    return m_masterVisible;
}

void PkAction::setMasterEnabled(bool value)
{
    m_masterEnabled = value;
    updateAction();
}

bool PkAction::masterEnabled() const
{
    return m_masterEnabled;
}

#include "polkit_qt_action.moc"
