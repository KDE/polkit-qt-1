// This is an example not a library
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
#include <ActionButtons>
#include <Context>
#include <Auth>
#include <QDebug>

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

using namespace PolkitQt;

PkExample::PkExample(QMainWindow *parent)
        : QMainWindow(parent)
{
    setupUi(this);

    ActionButton *bt;

    // Here we create an ActionButton that is a subclass of Action
    // always pass a QAbstractButton pointer and action id
    // You can change the action id later if you want
    bt = new ActionButton(playPB, "org.qt.policykit.examples.play", this);
    // Here we are setting the text to all four states an action might have
    bt->setText("Play!");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    // As ActionButton is also an Action we cast it to add to menu
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    // this signal is emitted when the user click on the action,
    // it will only happen if it was inserted in a QMenu or a QToolBar
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    // This signal was propagated from the QAbstractButton just for
    // convenience in this case we don't have any benefit but the code
    // look cleaner
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    // this is the Action activated signal, it is always emmited whenever
    // someone click and get authorized to do the action
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(cryPB, "org.qt.policykit.examples.cry", this);
    bt->setText("Cry!");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    // This action is more customized
    bt = new ActionButton(kickPB, "org.qt.policykit.examples.kick", this);
    bt->setText("Kick... (long)");
    // here we set the behavior of PolKitResul = No
    bt->setNoVisible(true);
    bt->setNoEnabled(true);
    bt->setNoText("Kick (long)");
    bt->setNoIcon(QPixmap(":/Icons/custom-no.png"));
    bt->setNoToolTip("If your admin wasn't annoying, you could do this");
    // here we set the behavior of PolKitResul = Auth
    bt->setAuthVisible(true);
    bt->setAuthEnabled(true);
    bt->setAuthText("Kick... (long)");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    bt->setAuthToolTip("Only card carrying tweakers can do this!");
    // here we set the behavior of PolKitResul = Yes
    bt->setYesVisible(true);
    bt->setYesEnabled(true);
    bt->setYesText("Kick! (long)");
    bt->setYesIcon(QPixmap(":/Icons/custom-yes.png"));
    bt->setYesToolTip("Go ahead, kick kick kick!");

    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(deletePB, "org.qt.policykit.examples.delete", this);
    bt->setText("Delete!");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(bleedPB, "org.qt.policykit.examples.bleed", this);
    bt->setText("Bleed!");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButtons(QList<QAbstractButton*>() << listenPB << listenCB,
                           "org.qt.policykit.examples.listen", this);
    bt->setIcon(QPixmap(":/Icons/action-locked.png"));
    bt->setYesIcon(QPixmap(":/Icons/action-unlocked.png"));
    bt->setText("Click to make changes...");
    // this example is pretty diferent, here we have a checkable
    // QAbstractButton, the setCheckable(true) was set in the ui
    // file so you can see that there is no need to call
    // bt->setCheckable(true);
    // if you want a simple Action checkable you can do as
    // of above.
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)),
            this, SLOT(activateCheckableAction()));
    // We have here a separated slot cause we want to revoke the
    // action.
    // Be sure not to cast sender to Action if you do so
    // you will call the wrong activate slot and it won't work
    // well with checkable actions
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)),
            this, SLOT(activateCheckableAction()));
    connect(bt, SIGNAL(activated()),
            this, SLOT(actionActivated()));
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
    // be correct parented with your application.
    action->activate(winId());
}

// activateCheckableAction is only here to revoke the action
// As the slot above you might not need both of them, just
// connect to the activat slot of an Action or ActionButton
void PkExample::activateCheckableAction()
{
    // Here we cast the sender() to an ActionButton and call activate()
    // on it.
    // You will probable want to connect it derectly to your app,
    // here we do this way since we want to revoke the action
    // if we are granted it
    ActionButton *action = qobject_cast<ActionButton*>(sender());
    if (action->isAllowed()) {
        action->revoke();
    } else {
        // calling activate with winId() makes the auth dialog
        // be correct parented with your application.
        action->activate();
    }
}

void PkExample::actionActivated()
{
    // This slot is called whenever an action is allowed
    // here you will do your dirt job by calling a helper application
    // that might erase your hardrive ;)
    Action *action = qobject_cast<Action*>(sender());

    // here we don't want to do nothing if the action is listen
    if (action->is("org.qt.policykit.examples.listen")) {
        qDebug() << "toggled for: org.qt.policykit.examples.listen";
        return;
    }

    // this is our Special Action that after allowed will call the method
    if (action->is("org.qt.policykit.examples.play")) {
        qDebug() << "toggled for: org.qt.policykit.examples.play";

        QDBusMessage message;
        message = QDBusMessage::createMethodCall("org.qt.policykit.examples",
                  "/",
                  "org.qt.policykit.examples",
                  QLatin1String("play"));
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(QString("Daniel Nicoletti"));
        message.setArguments(argumentList);
        // notice the systemBus here..
        QDBusMessage reply = QDBusConnection::systemBus().call(message);
        if (reply.type() == QDBusMessage::ReplyMessage
                && reply.arguments().size() == 1) {
            // the reply can be anything, here we receive a string
            qDebug() << reply.arguments().first().toString();
        } else if (reply.type() == QDBusMessage::MethodCallMessage) {
            qWarning() << "Message did not receive a reply (timeout by message bus)";
        }
        return;
    }

    // As debug message says we are pretending to be the mechanism for the
    // following action, here you will actually call your DBus helper that
    // will run as root (setuid is not needed, see DBus docs).
    // In the helper application you will issue isCallerAuthorized,
    // passing the action id, the caller pid (which DBus will tell you) and
    // revokeIfOneShot = true as OneShot actions requires that the helper
    // revoke it after it sees that it's possible to do it, otherwise it
    // will work forever AND THAT IS NOT WHAT you want! If it's is simple
    // don't use oneShot in you .policy file ;)
    qDebug() << "pretending to be the mechanism for action:" << action->actionId();

    // note how we pass true to revokeIfOneShot - this is because we're
    // pretending to be the mechanism
    Auth::Result result;
    result = Auth::isCallerAuthorized(action->actionId(), static_cast<uint>(QCoreApplication::applicationPid()), true);
    if (result == Auth::Yes) {
        // in the helper you will do the action
        qDebug() << "caller is authorized to do:" << action->actionId();
    } else {
        // OR return false to the caller then the caller can ask for an auth dialog
        // or do anything else
        qDebug() << "caller is NOT authorized to do:" << action->actionId();
    }
}
