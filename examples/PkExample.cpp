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

#include <ActionButton>
#include <Context>
#include <QDebug>

using namespace PolkitQt;

PkExample::PkExample(QMainWindow *parent)
        : QMainWindow(parent)
{
    setupUi(this);

    ActionButton *bt;

    // Here we create an ActionButton that is a subclass of Action
    // allways pass a QAbstractButton pointer and action id
    // You can change the action id later if you want
    bt = new ActionButton(playPB, "org.qt.policykit.examples.play", this);
    // Here we are setting the text to all four states an action might have
    bt->setText("Play!");
    playPB->setIcon(QPixmap(":/Icons/action-locked-default.png"));
    // As ActionButton is also an Action we cast it to add to menu
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    // this signal is emitted when the user click on the action,
    // it will only happen if it was inserted in a QMenu or a QToolBar
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    // This signal was propagated from the QAbstractButton just for
    // convenience in this case we don't have any benefit but the code
    // look cleaner
    connect(bt, SIGNAL(clicked(bool)), bt, SLOT(activate()));
    // this is the Action activated signal, it is always emmited whenever
    // someone click and get authorized to do the action
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(cryPB, "org.qt.policykit.examples.cry", this);
    bt->setText("Cry!");
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    // This action is more customized
    bt = new ActionButton(kickPB, "org.qt.policykit.examples.kick", this);
    bt->setText("Kick... (long)");
    // here we set the behavior of PolKitResul = No
    bt->setNoVisible(true);
    bt->setNoEnabled(true);
    bt->setNoText("Kick (long)");
    bt->setNoToolTip("If your admin wasn't annoying, you could do this");
    // here we set the behavior of PolKitResul = Auth
    bt->setAuthVisible(true);
    bt->setAuthEnabled(true);
    bt->setAuthText("Kick... (long)");
    bt->setAuthToolTip("Only card carrying tweakers can do this!");
    // here we set the behavior of PolKitResul = Yes
    bt->setYesVisible(true);
    bt->setYesEnabled(true);
    bt->setYesText("Kick! (long)");
    bt->setYesToolTip("Go ahead, kick kick kick!");

    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(deletePB, "org.qt.policykit.examples.delete", this);
    bt->setText("Delete!");
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(bleedPB, "org.qt.policykit.examples.bleed", this);
    bt->setText("Bleed!");
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(listenPB, "org.qt.policykit.examples.listen", this);
    bt->setText("Click to make changes...");
    // this example is pretty diferent, here we have a checkable
    // QAbstractButton, the setCheckable(true) was set in the ui
    // file so you can see that there is no need to call
    // bt->setCheckable(true);
    // if you want a simple Action checkable you can do as
    // of above.
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));
}

PkExample::~PkExample()
{
}

void PkExample::activateAction()
{
    // Here we cast the sender() to an Action and call activate()
    // on it.
    // Be careful in doing the same for ActionButton won't work as expected
    // as action->activate() is calling Action::activate() and
    // not ActionButton::activate which are different.
    // You can cast then to ActionButton but be carefull
    // an Action casted to ActionButton may crash you app
    Action *action = qobject_cast<Action*>(sender());
    // calling activate with winId() makes the auth dialog
    // be correct parented with you application.
    action->activate(winId());
}

void PkExample::actionActivated()
{
    // This slot is called whenever an action is allowed
    // here you will do your dirt job by calling a helper application
    // that might erase your hardrive ;)
    Action *action = qobject_cast<Action*>(sender());

    PolKitCaller *pk_caller;
    PolKitAction *pk_action;
    PolKitResult pk_result;
    DBusError dbus_error;

    if (action->is("org.qt.policykit.examples.listen")) {
        qDebug() << "toggled for: org.qt.policykit.examples.listen";
        return;
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
        // note how we pass true to revoke_if_one_shot - this is because we're
        // pretending to be the mechanism
        pk_result = polkit_context_is_caller_authorized (Context::instance()->getPolKitContext(),
                                                            pk_action,
                                                            pk_caller,
                                                            true,
                                                            NULL);
        polkit_caller_unref (pk_caller);
    }

    polkit_action_unref (pk_action);
}
