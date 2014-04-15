/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf@kde.org>
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

#include "polkitqt1-gui-action.h"
#include "polkitqt1-authority.h"
#include "polkitqt1-subject.h"

#include <QtCore/QCoreApplication>

namespace PolkitQt1
{

namespace Gui
{

/**
  * \internal
  */
class Action::Private
{
public:
    Private(Action *p);

    Action *parent;

    QString       actionId;
    Authority::Result  pkResult;
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
    case Authority::Yes:
    case Authority::Challenge:
        // just Q_EMIT the 'activated' signal
        Q_EMIT authorized();
        return true;
        break;
    default:
    case Authority::No:
        if (d->noEnabled) {
            /* If PolicyKit says no... and we got here.. it means
             * that the user set the property "no-enabled" to
             * TRUE..
             *
             * Hence, they probably have a good reason for doing
             * this so do let the 'activate' signal propagate..
             */
            Q_EMIT authorized();
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
        case Authority::Unknown:
        case Authority::No:
            qobject_cast<QAction *>(parent)->setVisible(noVisible);
            qobject_cast<QAction *>(parent)->setEnabled(noEnabled);
            qobject_cast<QAction *>(parent)->setText(noText);
            if (!noWhatsThis.isNull()) {
                qobject_cast<QAction *>(parent)->setWhatsThis(noWhatsThis);
            }
            if (!noToolTip.isNull()) {
                qobject_cast<QAction *>(parent)->setToolTip(noToolTip);
            }
            qobject_cast<QAction *>(parent)->setIcon(noIcon);
            break;

        case Authority::Challenge:
            qobject_cast<QAction *>(parent)->setVisible(authVisible);
            qobject_cast<QAction *>(parent)->setEnabled(authEnabled);
            qobject_cast<QAction *>(parent)->setText(authText);
            if (!authWhatsThis.isNull()) {
                qobject_cast<QAction *>(parent)->setWhatsThis(authWhatsThis);
            }
            if (!authToolTip.isNull()) {
                qobject_cast<QAction *>(parent)->setToolTip(authToolTip);
            }
            qobject_cast<QAction *>(parent)->setIcon(authIcon);
            break;
        case Authority::Yes:
            qobject_cast<QAction *>(parent)->setVisible(yesVisible);
            qobject_cast<QAction *>(parent)->setEnabled(yesEnabled);
            qobject_cast<QAction *>(parent)->setText(yesText);
            if (!yesWhatsThis.isNull()) {
                qobject_cast<QAction *>(parent)->setWhatsThis(yesWhatsThis);
            }
            if (!yesToolTip.isNull()) {
                qobject_cast<QAction *>(parent)->setToolTip(yesToolTip);
            }
            qobject_cast<QAction *>(parent)->setIcon(yesIcon);
            if (parent->isCheckable()) {
                qobject_cast<QAction *>(parent)->setChecked(!initiallyChecked);
            }
            break;
    }
    Q_EMIT parent->dataChanged();
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
    Authority::Result old_result;
    UnixProcessSubject subject(parent->targetPID());

    old_result = pkResult;
    pkResult = Authority::Unknown;

    pkResult = Authority::instance()->checkAuthorizationSync(actionId, subject, Authority::None);

    return old_result != pkResult;
}

qint64 Action::targetPID() const
{
    if (d->targetPID != 0) {
        return d->targetPID;
    } else {
        return QCoreApplication::applicationPid();
    }
}

void Action::setTargetPID(qint64 pid)
{
    d->targetPID = pid;

    d->computePkResult();
    d->updateAction();
}

bool Action::isAllowed() const
{
    return d->pkResult == Authority::Yes;
}

bool Action::is(const QString &other) const
{
    return d->actionId == other;
}

void Action::revoke()
{
    /*TODO: implement it? no negative authorizations available, no authorization db*/
}

void Action::setText(const QString &text, States states)
{
    if (states & All) {
        d->selfBlockedText = text;
        d->noText = text;
        d->authText = text;
        d->yesText = text;
    } else if (states & Auth) {
        d->authText = text;
    } else if (states & No) {
        d->noText = text;
    } else if (states & SelfBlocked) {
        d->selfBlockedText = text;
    } else if (states & Yes) {
        d->yesText = text;
    }

    d->updateAction();
}

QString Action::text(Action::State state) const
{
    switch (state) {
        case Yes:
            return d->yesText;
        case No:
            return d->noText;
        case Auth:
            return d->authText;
        case SelfBlocked:
            return d->selfBlockedText;
        case None:
            return QAction::text();
        default:
            return QString();
    }
}

void Action::setToolTip(const QString &toolTip, States states)
{
    if (states & All) {
        d->selfBlockedToolTip = toolTip;
        d->noToolTip = toolTip;
        d->authToolTip = toolTip;
        d->yesToolTip = toolTip;
    } else if (states & Auth) {
        d->authToolTip = toolTip;
    } else if (states & No) {
        d->noToolTip = toolTip;
    } else if (states & SelfBlocked) {
        d->selfBlockedToolTip = toolTip;
    } else if (states & Yes) {
        d->yesToolTip = toolTip;
    }

    d->updateAction();
}

QString Action::toolTip(Action::State state) const
{
    switch (state) {
        case Yes:
            return d->yesToolTip;
        case No:
            return d->noToolTip;
        case Auth:
            return d->authToolTip;
        case SelfBlocked:
            return d->selfBlockedToolTip;
        case None:
            return QAction::toolTip();
        default:
            return QString();
    }
}

void Action::setWhatsThis(const QString &whatsThis, States states)
{
    if (states & All) {
        d->selfBlockedWhatsThis = whatsThis;
        d->noWhatsThis = whatsThis;
        d->authWhatsThis = whatsThis;
        d->yesWhatsThis = whatsThis;
    } else if (states & Auth) {
        d->authWhatsThis = whatsThis;
    } else if (states & No) {
        d->noWhatsThis = whatsThis;
    } else if (states & SelfBlocked) {
        d->selfBlockedWhatsThis = whatsThis;
    } else if (states & Yes) {
        d->yesWhatsThis = whatsThis;
    }

    d->updateAction();
}

QString Action::whatsThis(Action::State state) const
{
    switch (state) {
        case Yes:
            return d->yesWhatsThis;
        case No:
            return d->noWhatsThis;
        case Auth:
            return d->authWhatsThis;
        case SelfBlocked:
            return d->selfBlockedWhatsThis;
        case None:
            return QAction::whatsThis();
        default:
            return QString();
    }
}

void Action::setIcon(const QIcon &icon, States states)
{
    if (states & All) {
        d->selfBlockedIcon = icon;
        d->noIcon = icon;
        d->authIcon = icon;
        d->yesIcon = icon;
    } else if (states & Auth) {
        d->authIcon = icon;
    } else if (states & No) {
        d->noIcon = icon;
    } else if (states & SelfBlocked) {
        d->selfBlockedIcon = icon;
    } else if (states & Yes) {
        d->yesIcon = icon;
    }

    d->updateAction();
}

QIcon Action::icon(Action::State state) const
{
    switch (state) {
        case Yes:
            return d->yesIcon;
        case No:
            return d->noIcon;
        case Auth:
            return d->authIcon;
        case SelfBlocked:
            return d->selfBlockedIcon;
        case None:
            return QAction::icon();
        default:
            return QIcon();
    }
}

void Action::setEnabled(bool enabled, States states)
{
    if (states & All) {
        d->selfBlockedEnabled = enabled;
        d->noEnabled = enabled;
        d->authEnabled = enabled;
        d->yesEnabled = enabled;
    } else if (states & Auth) {
        d->authEnabled = enabled;
    } else if (states & No) {
        d->noEnabled = enabled;
    } else if (states & SelfBlocked) {
        d->selfBlockedEnabled = enabled;
    } else if (states & Yes) {
        d->yesEnabled = enabled;
    }

    d->updateAction();
}

bool Action::isEnabled(Action::State state) const
{
    switch (state) {
        case Yes:
            return d->yesEnabled;
        case No:
            return d->noEnabled;
        case Auth:
            return d->authEnabled;
        case SelfBlocked:
            return d->selfBlockedEnabled;
        case None:
            return QAction::isEnabled();
        default:
            return false;
    }
}

void Action::setVisible(bool visible, States states)
{
    if (states & All) {
        d->selfBlockedVisible = visible;
        d->noVisible = visible;
        d->authVisible = visible;
        d->yesVisible = visible;
    } else if (states & Auth) {
        d->authVisible = visible;
    } else if (states & No) {
        d->noVisible = visible;
    } else if (states & SelfBlocked) {
        d->selfBlockedVisible = visible;
    } else if (states & Yes) {
        d->yesVisible = visible;
    }

    d->updateAction();
}

bool Action::isVisible(Action::State state) const
{
    switch (state) {
        case Yes:
            return d->yesVisible;
        case No:
            return d->noVisible;
        case Auth:
            return d->authVisible;
        case SelfBlocked:
            return d->selfBlockedVisible;
        case None:
            return QAction::isVisible();
        default:
            return false;
    }
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

}

}

#include "moc_polkitqt1-gui-action.cpp"
