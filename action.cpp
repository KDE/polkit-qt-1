/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
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
#include "authority.h"
#include "auth.h"
#include "subject.h"

#include <polkit/polkit.h>
#include <QDebug>

using namespace PolkitQt;

class Action::Private
{
public:
    Private(Action *p);

    Action *parent;

    QString       actionId;
    Auth::Result  pkResult;
    qint64        targetPID;

    void                 updateAction();
    bool                 computePkResult();
    void                 configChanged();

    bool    initiallyChecked;

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
        , targetPID(getpid())
{
    initiallyChecked = false;

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
    connect(Authority::instance(), SIGNAL(configChanged()),
            this, SLOT(configChanged()));
    // for now we call config changed..
    connect(Authority::instance(), SIGNAL(consoleKitDBChanged()),
            this, SLOT(configChanged()));
}

Action::~Action()
{
    delete d;
}

bool Action::activate()
{
    switch (d->pkResult) {
    case Auth::Yes:
    case Auth::Challenge:
        // just emit the 'activated' signal
        emit activated();
        return true;
        break;
    default:
    case Auth::No: 
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

void Action::setChecked(bool checked)
{
    // We store this as initiallyChecked
    // to be able to undo changes in case the auth fails
    d->initiallyChecked = checked;
    QAction::setChecked(checked);
}

void Action::Private::updateAction()
{
    if (Authority::instance()->hasError()) {
        return;
    }

    switch (pkResult) {
    default:
    case Auth::Unknown:
    case Auth::No:
        parent->setVisible(noVisible && masterVisible);
        parent->setEnabled(noEnabled && masterEnabled);
        qobject_cast<QAction*>(parent)->setText(noText);
        if (!noWhatsThis.isNull()) {
            qobject_cast<QAction*>(parent)->setWhatsThis(noWhatsThis);
        }
        if (!noToolTip.isNull()) {
            qobject_cast<QAction*>(parent)->setToolTip(noToolTip);
        }
        qobject_cast<QAction*>(parent)->setIcon(noIcon);
        break;

    case Auth::Challenge:
        parent->setVisible(authVisible && masterVisible);
        parent->setEnabled(authEnabled && masterEnabled);
        qobject_cast<QAction*>(parent)->setText(authText);
        if (!authWhatsThis.isNull()) {
            qobject_cast<QAction*>(parent)->setWhatsThis(authWhatsThis);
        }
        if (!authToolTip.isNull()) {
            qobject_cast<QAction*>(parent)->setToolTip(authToolTip);
        }
        qobject_cast<QAction*>(parent)->setIcon(authIcon);
        break;
    case Auth::Yes:
        parent->setVisible(yesVisible && masterVisible);
        parent->setEnabled(yesEnabled && masterEnabled);
        qobject_cast<QAction*>(parent)->setText(yesText);
        if (!yesWhatsThis.isNull()) {
            qobject_cast<QAction*>(parent)->setWhatsThis(yesWhatsThis);
        }
        if (!yesToolTip.isNull()) {
            qobject_cast<QAction*>(parent)->setToolTip(yesToolTip);
        }
        qobject_cast<QAction*>(parent)->setIcon(yesIcon);
        if (parent->isCheckable()) {
            qobject_cast<QAction*>(parent)->setChecked(!initiallyChecked);
        }
        break;
    }
    emit parent->dataChanged();
}

void Action::Private::configChanged()
{
    bool result_changed;
    result_changed = computePkResult();
    if (result_changed) {
        updateAction();
    }
}

bool Action::Private::computePkResult()
{
    Auth::Result old_result;
    UnixProcess *subject;
  
    subject = new UnixProcess(parent->targetPID());
    
    old_result = pkResult;
    pkResult = Auth::Unknown;

    pkResult = Auth::checkAuthorization(actionId, subject, Auth::None);

    return old_result != pkResult;
}

qint64 Action::targetPID() const
{
    if (d->targetPID != 0)
        return d->targetPID;
    else
        return QCoreApplication::applicationPid();
}

void Action::setTargetPID(qint64 pid)
{
    d->targetPID = pid;

    d->computePkResult();
    d->updateAction();
}

bool Action::isAllowed() const
{
    return d->pkResult == Auth::Yes;
}

bool Action::is(const QString &other) const
{
    return d->actionId == other;
}

void Action::revoke()
{
/*TODO: implement it? no negative authorizations available, no authorization db*/
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

void Action::setPolkitAction(const QString &actionId)	 
{
    //TODO:
    d->actionId = actionId; 
    
    d->computePkResult();	 
    d->updateAction();
}

//--------------------------------------------------

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

#include "moc_action.cpp"
