/***************************************************************************
 *   Copyright (C) 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "PkExample.h"

#include <Context>
#include <QDebug>

using namespace PolkitQt;

PkExample::PkExample(QMainWindow *parent)
        : QMainWindow(parent)
{
    setupUi(this);

    playA = new Action("org.qt.policykit.examples.play", this);
    playA->setText("Play!");
    menuActions->addAction(playA);
    toolBar->addAction(playA);
    connect(playA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(playA, SIGNAL(activated()), this, SLOT(actionActivated()));

    cryA = new Action("org.qt.policykit.examples.cry", this);
    cryA->setText("Cry!");
    menuActions->addAction(cryA);
    toolBar->addAction(cryA);
    connect(cryA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(cryA, SIGNAL(activated()), this, SLOT(actionActivated()));

    kickA = new Action("org.qt.policykit.examples.kick", this);
    kickA->setText("Kick... (long)");
    kickA->setNoVisible(true);
    kickA->setNoEnabled(true);
    kickA->setNoText("Kick (long)");
    kickA->setNoToolTip("If your admin wasn't annoying, you could do this");

    kickA->setAuthVisible(true);
    kickA->setAuthEnabled(true);
    kickA->setAuthText("Kick... (long)");
    kickA->setAuthToolTip("Only card carrying tweakers can do this!");

    kickA->setYesVisible(true);
    kickA->setYesEnabled(true);
    kickA->setYesText("Kick! (long)");
    kickA->setYesToolTip("Go ahead, kick kick kick!");

    menuActions->addAction(kickA);
    toolBar->addAction(kickA);
    connect(kickA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(kickA, SIGNAL(activated()), this, SLOT(actionActivated()));

    deleteA = new Action("org.qt.policykit.examples.delete", this);
    deleteA->setText("Delete!");
    menuActions->addAction(deleteA);
    toolBar->addAction(deleteA);
    connect(deleteA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(deleteA, SIGNAL(activated()), this, SLOT(actionActivated()));

    bleedA = new Action("org.qt.policykit.examples.bleed", this);
    bleedA->setText("Bleed!");
    menuActions->addAction(bleedA);
    toolBar->addAction(bleedA);
    connect(bleedA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bleedA, SIGNAL(activated()), this, SLOT(actionActivated()));

    listenA = new Action("org.qt.policykit.examples.listen", this);
    listenA->setText("Click to make changes...");
    listenA->setCheckable(true);
    listenA->setChecked(true);
    menuActions->addAction(listenA);
    toolBar->addAction(listenA);
    connect(listenA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(listenA, SIGNAL(activated()), this, SLOT(actionActivated()));
}

PkExample::~PkExample()
{
}

void PkExample::activateAction()
{
    Action *action = (Action *) sender(); 
    action->activate(winId());
}

void PkExample::actionActivated()
{
    Action *action = (Action *) sender();

    PolKitCaller *pk_caller;
    PolKitAction *pk_action;
    PolKitResult pk_result;
    DBusError dbus_error;

    if (action->is("org.qt.policykit.examples.listen")) {
        qDebug() <<"org.qt.policykit.examples.listen";
        action->revoke();
    }

    qDebug() << "pretending to be the mechanism for action:" << action->actionId();

    pk_action = action->polkitAction();

    dbus_error_init (&dbus_error);
    pk_caller = polkit_tracker_get_caller_from_pid (Context::instance()->getPolKitTracker(),
                                                    getpid (),
                                                    &dbus_error);
    if (pk_caller == NULL) {
        qWarning() << "Cannot get PolKitCaller object for ourselves "
                      "(pid=" << getpid() << "): " << dbus_error.name << ": " << dbus_error.message;
        dbus_error_free (&dbus_error);
    } else {
        /* note how we pass #TRUE to revoke_if_one_shot - this is because we're
            * pretending to be the mechanism
            */
        pk_result = polkit_context_is_caller_authorized (Context::instance()->getPolKitContext(),
                                                            pk_action,
                                                            pk_caller,
                                                            TRUE,
                                                            NULL);

        polkit_caller_unref (pk_caller);
    }

    polkit_action_unref (pk_action);
}
