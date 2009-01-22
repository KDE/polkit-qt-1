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

//     PolKitCaller *pk_caller;
//     DBusError dbus_error;

    // Set the default values
    self_blocked_visible     = true;
    self_blocked_sensitive   = false;
    self_blocked_text        = text;
    self_blocked_short_text  = tooltip;
    self_blocked_tooltip     = tooltip;
//     self_blocked_icon_name   = QIcon();

    no_visible       = true;
    no_sensitive     = false;
    no_text          = text;
    no_short_text    = tooltip;
    no_tooltip       = tooltip;
//     no_icon_name     = QIcon();

    auth_visible     = true;
    auth_sensitive   = true;
    auth_text        = text;
    auth_short_text  = tooltip;
    auth_tooltip     = tooltip;
//     auth_icon_name   = GTK_STOCK_DIALOG_AUTHENTICATION;

    yes_visible      = true;
    yes_sensitive    = true;
    yes_text         = text;
    yes_short_text   = tooltip;
    yes_tooltip      = tooltip;
//     yes_icon_name    = QIcon();

    master_visible   = true;
    master_sensitive = true;

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
            emit activated();
            break;
    }
}

void PkAction::updateAction()
{
        PolKitAuthorizationDB *authdb;
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
                    m_visible = self_blocked_visible && master_visible;
                    m_sensitive = self_blocked_sensitive && master_sensitive;
                    m_whatsThis = self_blocked_short_text;
                    m_text = self_blocked_text;
                    m_tooltip = self_blocked_tooltip;
                    m_iconName = self_blocked_icon_name;
                } else {
                    m_visible = no_visible && master_visible;
                    m_sensitive = no_sensitive && master_sensitive;
                    m_whatsThis = no_short_text;
                    m_text = no_text;
                    m_tooltip = no_tooltip;
                    m_iconName = no_icon_name;
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
                m_visible = auth_visible && master_visible;
                m_sensitive = auth_sensitive && master_sensitive;
                m_whatsThis = auth_short_text;
                m_text = auth_text;
                m_tooltip = auth_tooltip;
                m_iconName = auth_icon_name;
                break;

            case POLKIT_RESULT_YES:
                m_visible = yes_visible && master_visible;
                m_sensitive = yes_sensitive && master_sensitive;
                m_whatsThis = yes_short_text;
                m_text = yes_text;
                m_tooltip = yes_tooltip;
                m_iconName = yes_icon_name;
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
qDebug() << "computePkResult 1";
    if (m_pkAction == 0) {
        m_pkResult = POLKIT_RESULT_YES;
    } else {
    qDebug() << "computePkResult 2";
        m_pkResult = computePkResultDirect(m_pkAction, targetPID());
    }
qDebug() << "computePkResult 3";
    return old_result != m_pkResult;
}

PolKitResult PkAction::computePkResultDirect(PolKitAction *action, pid_t pid)
{
    PolKitCaller *pk_caller;
    PolKitResult pk_result;
    DBusError dbus_error;
    // TODO
//     _ensure_pk_g_context (action);
qDebug() << "computePkResultDirect 1";
    dbus_error_init (&dbus_error);
    pk_caller = polkit_tracker_get_caller_from_pid (PkContext::instance()->pkTracker,
                                                    pid,
                                                    &dbus_error);
qDebug() << "computePkResultDirect 2";
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
qDebug() << "computePkResultDirect 3";
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

void PkAction::setPolkiAction(const QString &action_id)
{
qDebug() << "setPolkiAction";
    PolKitAction *pkAction = polkit_action_new ();
    qDebug() << "hello";
    qDebug() << action_id.toLocal8Bit().data();
    polkit_action_set_action_id (pkAction, action_id.toAscii().data());
    /* Don't bother updating polkit_action if it's the same
     * value.. it will just cause a lot of unnecessary work as
     * we'll recompute the answer via PolicyKit..
     *
     * unless it's on the initial call (where priv->polkit_action
     * is alread NULL) because we need that initial update;
     */
     qDebug() << "hello1";
    if (!m_pkAction || m_pkAction != pkAction) {
qDebug() << "hello2";
//                 action->priv->polkit_action_set_once = TRUE;

        if (m_pkAction != 0)
                polkit_action_unref (m_pkAction);
qDebug() << "hello3";
        if (pkAction != NULL) {
            m_pkAction = polkit_action_ref (pkAction);
            m_actionId = action_id;
        } else {
            m_pkAction = 0;
            m_actionId.clear();
        }
qDebug() << "hello4";
        computePkResult();
qDebug() << "hello5";
        updateAction();
    }
    qDebug() << "visible" << m_visible;
    qDebug() << "sensitive" << m_sensitive;
    qDebug() << "setPolkiAction end";

}

void PkAction::setSelfBlockedVisible(bool value)
{
    self_blocked_visible = value;
    updateAction();
}

bool PkAction::selfBlockedVisible() const
{
    return self_blocked_visible;
}

void PkAction::setSelfBlockedEnabled(bool value)
{
    self_blocked_sensitive = value;
    updateAction();
}

bool PkAction::selfBlockedEnabled() const
{
    return self_blocked_sensitive;
}

void PkAction::setSelfBlockedText(const QString &text)
{
    self_blocked_text = text;
    updateAction();
}

QString PkAction::selfBlockedText() const
{
    return self_blocked_text;
}

void PkAction::setSelfBlockedToolTip(const QString &toolTip)
{
    self_blocked_tooltip = toolTip;
    updateAction();
}

QString PkAction::selfBlockedToolTip() const
{
    return self_blocked_tooltip;
}

void PkAction::setSelfBlockedWhatsThis(const QString &whatsThis)
{
    self_blocked_short_text = whatsThis;
    updateAction();
}

QString PkAction::selfBlockedWhatsThis() const
{
    return self_blocked_short_text;
}

void PkAction::setSelfBlockedIconName(const QString &iconName)
{
    self_blocked_icon_name = iconName;
    updateAction();
}

QString PkAction::selfBlockedIconName() const
{
    return self_blocked_icon_name;
}
//----------------------------------------------------------
void PkAction::setNoVisible(bool value)
{
    no_visible = value;
    updateAction();
}

bool PkAction::noVisible() const
{
    return no_visible;
}

void PkAction::setNoEnabled(bool value)
{
    no_sensitive = value;
    updateAction();
}

bool PkAction::noEnabled() const
{
    return no_sensitive;
}

void PkAction::setNoText(const QString &text)
{
    no_text = text;
    updateAction();
}

QString PkAction::noText() const
{
    return no_text;
}

void PkAction::setNoToolTip(const QString &toolTip)
{
    no_tooltip = toolTip;
    updateAction();
}

QString PkAction::noToolTip() const
{
    return no_tooltip;
}

void PkAction::setNoWhatsThis(const QString &whatsThis)
{
    no_short_text = whatsThis;
    updateAction();
}

QString PkAction::noWhatsThis() const
{
    return no_short_text;
}

void PkAction::setNoIconName(const QString &iconName)
{
    no_icon_name = iconName;
    updateAction();
}

QString PkAction::noIconName() const
{
    return no_icon_name;
}
//-----------------------------------------
void PkAction::setAuthVisible(bool value)
{
    auth_visible = value;
    updateAction();
}

bool PkAction::authVisible() const
{
    return auth_visible;
}

void PkAction::setAuthEnabled(bool value)
{
    auth_sensitive = value;
    updateAction();
}

bool PkAction::authEnabled() const
{
    return auth_sensitive;
}

void PkAction::setAuthText(const QString &text)
{
    auth_text = text;
    updateAction();
}

QString PkAction::authText() const
{
    return auth_text;
}

void PkAction::setAuthToolTip(const QString &toolTip)
{
    auth_tooltip = toolTip;
    updateAction();
}

QString PkAction::authToolTip() const
{
    return auth_tooltip;
}

void PkAction::setAuthWhatsThis(const QString &whatsThis)
{
    auth_short_text = whatsThis;
    updateAction();
}

QString PkAction::authWhatsThis() const
{
    return auth_short_text;
}

void PkAction::setAuthIconName(const QString &iconName)
{
    auth_icon_name = iconName;
    updateAction();
}

QString PkAction::authIconName() const
{
    return auth_icon_name;
}
//-------------------------------------------------
void PkAction::setYesVisible(bool value)
{
    yes_visible = value;
    updateAction();
}

bool PkAction::yesVisible() const
{
    return yes_visible;
}

void PkAction::setYesEnabled(bool value)
{
    yes_sensitive = value;
    updateAction();
}

bool PkAction::yesEnabled() const
{
    return yes_sensitive;
}

void PkAction::setYesText(const QString &text)
{
    yes_text = text;
    updateAction();
}

QString PkAction::yesText() const
{
    return yes_text;
}

void PkAction::setYesToolTip(const QString &toolTip)
{
    yes_tooltip = toolTip;
    updateAction();
}

QString PkAction::yesToolTip() const
{
    return yes_tooltip;
}

void PkAction::setYesWhatsThis(const QString &whatsThis)
{
    yes_short_text = whatsThis;
    updateAction();
}

QString PkAction::yesWhatsThis() const
{
    return yes_short_text;
}

void PkAction::setYesIconName(const QString &iconName)
{
    yes_icon_name = iconName;
    updateAction();
}

QString PkAction::yesIconName() const
{
    return yes_icon_name;
}
//------------------------------------------------------
void PkAction::setMasterVisible(bool value)
{
    master_visible = value;
    updateAction();
}

bool PkAction::masterVisible() const
{
    return master_visible;
}

void PkAction::setMasterEnabled(bool value)
{
    master_sensitive = value;
    updateAction();
}

bool PkAction::masterEnabled() const
{
    return master_sensitive;
}

#include "polkit_qt_action.moc"
