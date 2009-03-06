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

#include "action.h"
#include "context.h"
#include "auth.h"

#include <polkit-dbus/polkit-dbus.h>

using namespace PolkitQt;

class Action::Private
{
public:
    Private(Action *p);

    Action *parent;

    QString       actionId;
    PolKitAction *pkAction;
    PolKitResult  pkResult;
    uint          targetPID;

    void         updateAction();
    bool         computePkResult();

    // states data
    bool    selfBlockedVisible;
    bool    selfBlockedEnabled;
    QString selfBlockedText;
    QString selfBlockedWhatsThis;
    QString selfBlockedToolTip;
    QIcon   selfBlockedIcon;

    bool    noVisible;
    bool    noEnabled;
    QString noText;
    QString noWhatsThis;
    QString noToolTip;
    QIcon   noIcon;

    bool    authVisible;
    bool    authEnabled;
    QString authText;
    QString authWhatsThis;
    QString authToolTip;
    QIcon   authIcon;

    bool    yesVisible;
    bool    yesEnabled;
    QString yesText;
    QString yesWhatsThis;
    QString yesToolTip;
    QIcon   yesIcon;

    bool masterVisible;
    bool masterEnabled;
};

Action::Private::Private(Action *p)
        : parent(p)
        , pkAction(NULL)
        , targetPID(0)
{
    // Set the default values
    selfBlockedVisible = true;
    selfBlockedEnabled = false;

    noVisible     = true;
    noEnabled     = false;

    authVisible   = true;
    authEnabled   = true;

    yesVisible    = true;
    yesEnabled    = true;

    masterVisible = true;
    masterEnabled = true;
}

Action::Action(const QString &actionId, QObject *parent)
        : QAction(parent)
        , d(new Private(this))
{
    // this must be called AFTER the values initialization
    setPolkitAction(actionId);

    // track the config changes to update the action
    connect(Context::instance(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
}

Action::~Action()
{
    if (d->pkAction != NULL) {
        polkit_action_unref(d->pkAction);
    }

    delete d;
}

bool Action::activate(WId winId)
{
    switch (d->pkResult) {
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
        if (d->pkAction != NULL) {
            if (Auth::obtainAuth(d->actionId, winId, targetPID())) {
                // Make sure our result is up to date
                d->computePkResult();
                // emit activated as the obtain auth said it was ok
                emit activated();
                return true;
            }
        }
        break;

    default:
    case POLKIT_RESULT_NO:
        if (d->noEnabled) {
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

void Action::Private::updateAction()
{
    PolKitAuthorizationDB *authdb;
    if (Context::instance()->hasError()) {
        return;
    }
    authdb = polkit_context_get_authorization_db(Context::instance()->getPolKitContext());

    switch (pkResult) {
    default:
    case POLKIT_RESULT_UNKNOWN:
    case POLKIT_RESULT_NO:
        /* TODO: see if we're self-blocked */
        if (pkAction != NULL &&
                polkit_authorization_db_is_uid_blocked_by_self(authdb,
                        pkAction,
                        getuid(),
                        NULL)) {
            parent->setVisible(selfBlockedVisible && masterVisible);
            parent->setEnabled(selfBlockedEnabled && masterEnabled);
            ((QAction *) parent)->setText(selfBlockedText);
            if (!selfBlockedWhatsThis.isNull()) {
                ((QAction *) parent)->setWhatsThis(selfBlockedWhatsThis);
            }
            if (!selfBlockedToolTip.isNull()) {
                ((QAction *) parent)->setToolTip(selfBlockedToolTip);
            }
            if (!selfBlockedIcon.isNull()) {
                ((QAction *) parent)->setIcon(selfBlockedIcon);
            }
        } else {
            parent->setVisible(noVisible && masterVisible);
            parent->setEnabled(noEnabled && masterEnabled);
            ((QAction *) parent)->setText(noText);
            if (!noWhatsThis.isNull()) {
                ((QAction *) parent)->setWhatsThis(noWhatsThis);
            }
            if (!noToolTip.isNull()) {
                ((QAction *) parent)->setToolTip(noToolTip);
            }
            if (!noIcon.isNull()) {
                ((QAction *) parent)->setIcon(noIcon);
            }
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
        parent->setVisible(authVisible && masterVisible);
        parent->setEnabled(authEnabled && masterEnabled);
        ((QAction *) parent)->setText(authText);
        if (!authWhatsThis.isNull()) {
            ((QAction *) parent)->setWhatsThis(authWhatsThis);
        }
        if (!authToolTip.isNull()) {
            ((QAction *) parent)->setToolTip(authToolTip);
        }
        if (!authIcon.isNull()) {
            ((QAction *) parent)->setIcon(authIcon);
        }
        break;

    case POLKIT_RESULT_YES:
        parent->setVisible(yesVisible && masterVisible);
        parent->setEnabled(yesEnabled && masterEnabled);
        ((QAction *) parent)->setText(yesText);
        if (!yesWhatsThis.isNull()) {
            ((QAction *) parent)->setWhatsThis(yesWhatsThis);
        }
        if (!yesToolTip.isNull()) {
            ((QAction *) parent)->setToolTip(yesToolTip);
        }
        if (!yesIcon.isNull()) {
            ((QAction *) parent)->setIcon(yesIcon);
        }
        break;
    }
    emit parent->dataChanged();
}

void Action::configChanged()
{
    bool result_changed;
    result_changed = d->computePkResult();
    if (result_changed) {
        d->updateAction();
    }
}

bool Action::Private::computePkResult()
{
    PolKitResult old_result;

    old_result = pkResult;
    pkResult = POLKIT_RESULT_UNKNOWN;

    if (pkAction == NULL) {
        pkResult = POLKIT_RESULT_YES;
    } else {
        pkResult = Action::computePkResultDirect(pkAction, parent->targetPID());
    }

    return old_result != pkResult;
}

PolKitResult Action::computePkResultDirect(PolKitAction *action, pid_t pid)
{
    PolKitCaller *pk_caller;
    PolKitResult pk_result;
    DBusError dbus_error;
    dbus_error_init(&dbus_error);

    if (Context::instance()->hasError())
        return pk_result = POLKIT_RESULT_UNKNOWN;
    pk_caller = polkit_tracker_get_caller_from_pid(Context::instance()->getPolKitTracker(),
                pid,
                &dbus_error);
    if (pk_caller == NULL) {
        qWarning("Cannot get PolKitCaller object for target (pid=%d): %s: %s",
                 pid, dbus_error.name, dbus_error.message);
        dbus_error_free(&dbus_error);

        /* this is bad so cop-out to UKNOWN */
        pk_result = POLKIT_RESULT_UNKNOWN;
    } else {
        pk_result = polkit_context_is_caller_authorized(Context::instance()->getPolKitContext(),
                    action,
                    pk_caller,
                    FALSE,
                    NULL);
    }

    if (pk_caller != NULL)
        polkit_caller_unref(pk_caller);

    return pk_result;
}

pid_t Action::targetPID()
{
    if (d->targetPID != 0)
        return d->targetPID;
    else
        return QCoreApplication::applicationPid();
}

void Action::setTargetPID(pid_t pid)
{
    d->targetPID = pid;

    d->computePkResult();
    d->updateAction();
}

void Action::setPolkitAction(const QString &actionId)
{
    PolKitAction *pkAction = polkit_action_new();
    polkit_action_set_action_id(pkAction, actionId.toAscii().data());
    /* Don't bother updating polkit_action if it's the same
     * value.. it will just cause a lot of unnecessary work as
     * we'll recompute the answer via PolicyKit..
     *
     * unless it's on the initial call (where priv->polkit_action
     * is alread NULL) because we need that initial update;
     */
    if (!d->pkAction || d->pkAction != pkAction) {
        if (d->pkAction != NULL)
            polkit_action_unref(d->pkAction);
        if (pkAction != NULL) {
            d->pkAction = polkit_action_ref(pkAction);
            d->actionId = actionId;
        } else {
            d->pkAction = NULL;
            d->actionId.clear();
        }
        d->computePkResult();
        d->updateAction();
    }
}

bool Action::canDoAction() const
{
    return d->pkResult == POLKIT_RESULT_YES;
}

void Action::setText(const QString &text)
{
    d->selfBlockedText = text;
    d->noText          = text;
    d->authText        = text;
    d->yesText         = text;
    d->updateAction();
}

void Action::setToolTip(const QString &toolTip)
{
    d->selfBlockedToolTip = toolTip;
    d->noToolTip          = toolTip;
    d->authToolTip        = toolTip;
    d->yesToolTip         = toolTip;
    d->updateAction();
}

void Action::setWhatsThis(const QString &whatsThis)
{
    d->selfBlockedWhatsThis = whatsThis;
    d->noWhatsThis          = whatsThis;
    d->authWhatsThis        = whatsThis;
    d->yesWhatsThis         = whatsThis;
    d->updateAction();
}

void Action::setIcon(const QIcon &icon)
{
    d->selfBlockedIcon = icon;
    d->noIcon          = icon;
    d->authIcon        = icon;
    d->yesIcon         = icon;
    d->updateAction();
}

//--------------------------------------------------
PolKitAction* Action::polkitAction() const
{
    return d->pkAction;
}

QString Action::actionId() const
{
    return d->actionId;
}
//---------------------------------------------------
void Action::setSelfBlockedVisible(bool value)
{
    d->selfBlockedVisible = value;
    d->updateAction();
}

bool Action::selfBlockedVisible() const
{
    return d->selfBlockedVisible;
}

void Action::setSelfBlockedEnabled(bool value)
{
    d->selfBlockedEnabled = value;
    d->updateAction();
}

bool Action::selfBlockedEnabled() const
{
    return d->selfBlockedEnabled;
}

void Action::setSelfBlockedText(const QString &text)
{
    d->selfBlockedText = text;
    d->updateAction();
}

QString Action::selfBlockedText() const
{
    return d->selfBlockedText;
}

void Action::setSelfBlockedToolTip(const QString &toolTip)
{
    d->selfBlockedToolTip = toolTip;
    d->updateAction();
}

QString Action::selfBlockedToolTip() const
{
    return d->selfBlockedToolTip;
}

void Action::setSelfBlockedWhatsThis(const QString &whatsThis)
{
    d->selfBlockedWhatsThis = whatsThis;
    d->updateAction();
}

QString Action::selfBlockedWhatsThis() const
{
    return d->selfBlockedWhatsThis;
}

void Action::setSelfBlockedIcon(const QIcon &icon)
{
    d->selfBlockedIcon = icon;
    d->updateAction();
}

QIcon Action::selfBlockedIcon() const
{
    return d->selfBlockedIcon;
}
//----------------------------------------------------------
void Action::setNoVisible(bool value)
{
    d->noVisible = value;
    d->updateAction();
}

bool Action::noVisible() const
{
    return d->noVisible;
}

void Action::setNoEnabled(bool value)
{
    d->noEnabled = value;
    d->updateAction();
}

bool Action::noEnabled() const
{
    return d->noEnabled;
}

void Action::setNoText(const QString &text)
{
    d->noText = text;
    d->updateAction();
}

QString Action::noText() const
{
    return d->noText;
}

void Action::setNoToolTip(const QString &toolTip)
{
    d->noToolTip = toolTip;
    d->updateAction();
}

QString Action::noToolTip() const
{
    return d->noToolTip;
}

void Action::setNoWhatsThis(const QString &whatsThis)
{
    d->noWhatsThis = whatsThis;
    d->updateAction();
}

QString Action::noWhatsThis() const
{
    return d->noWhatsThis;
}

void Action::setNoIcon(const QIcon &icon)
{
    d->noIcon = icon;
    d->updateAction();
}

QIcon Action::noIcon() const
{
    return d->noIcon;
}
//-----------------------------------------
void Action::setAuthVisible(bool value)
{
    d->authVisible = value;
    d->updateAction();
}

bool Action::authVisible() const
{
    return d->authVisible;
}

void Action::setAuthEnabled(bool value)
{
    d->authEnabled = value;
    d->updateAction();
}

bool Action::authEnabled() const
{
    return d->authEnabled;
}

void Action::setAuthText(const QString &text)
{
    d->authText = text;
    d->updateAction();
}

QString Action::authText() const
{
    return d->authText;
}

void Action::setAuthToolTip(const QString &toolTip)
{
    d->authToolTip = toolTip;
    d->updateAction();
}

QString Action::authToolTip() const
{
    return d->authToolTip;
}

void Action::setAuthWhatsThis(const QString &whatsThis)
{
    d->authWhatsThis = whatsThis;
    d->updateAction();
}

QString Action::authWhatsThis() const
{
    return d->authWhatsThis;
}

void Action::setAuthIcon(const QIcon &icon)
{
    d->authIcon = icon;
    d->updateAction();
}

QIcon Action::authIcon() const
{
    return d->authIcon;
}
//-------------------------------------------------
void Action::setYesVisible(bool value)
{
    d->yesVisible = value;
    d->updateAction();
}

bool Action::yesVisible() const
{
    return d->yesVisible;
}

void Action::setYesEnabled(bool value)
{
    d->yesEnabled = value;
    d->updateAction();
}

bool Action::yesEnabled() const
{
    return d->yesEnabled;
}

void Action::setYesText(const QString &text)
{
    d->yesText = text;
    d->updateAction();
}

QString Action::yesText() const
{
    return d->yesText;
}

void Action::setYesToolTip(const QString &toolTip)
{
    d->yesToolTip = toolTip;
    d->updateAction();
}

QString Action::yesToolTip() const
{
    return d->yesToolTip;
}

void Action::setYesWhatsThis(const QString &whatsThis)
{
    d->yesWhatsThis = whatsThis;
    d->updateAction();
}

QString Action::yesWhatsThis() const
{
    return d->yesWhatsThis;
}

void Action::setYesIcon(const QIcon &icon)
{
    d->yesIcon = icon;
    d->updateAction();
}

QIcon Action::yesIcon() const
{
    return d->yesIcon;
}
//------------------------------------------------------
void Action::setMasterVisible(bool value)
{
    d->masterVisible = value;
    d->updateAction();
}

bool Action::masterVisible() const
{
    return d->masterVisible;
}

void Action::setMasterEnabled(bool value)
{
    d->masterEnabled = value;
    d->updateAction();
}

bool Action::masterEnabled() const
{
    return d->masterEnabled;
}
