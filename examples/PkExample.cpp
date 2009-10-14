// This is an example not a library
/***************************************************************************
 *   Copyright (C) 2009 Radek Novacek    <rnovacek@redhat.com>             *
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

#include "actionbutton.h"
#include "actionbuttons.h"
#include "authority.h"
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
    bt = new ActionButton(kickPB, "org.qt.policykit.examples.kick", this);
    // Here we are setting the text and icon to all four states
    // an action might have
    bt->setText("Kick!");
    bt->setIcon(QPixmap(":/Icons/custom-no.png"));
    // By using set{Yes|No|Auth}Enabled you can set the states
    // when the button is enabled
    bt->setNoEnabled(true);
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
    bt->setIcon(QPixmap(":/Icons/custom-yes.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(bleedPB, "org.qt.policykit.examples.bleed", this);
    bt->setText("Bleed!");
    bt->setIcon(QPixmap(":/Icons/action-locked-default.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    // This action is more customized
    bt = new ActionButton(playPB, "org.qt.policykit.examples.play", this);
    bt->setText("Play!");
    // here we set the behavior of PolKitResul = No
    bt->setNoVisible(true);
    bt->setNoEnabled(true);
    bt->setNoText("Can't play!");
    bt->setNoIcon(QPixmap(":/Icons/custom-no.png"));
    bt->setNoToolTip("If your admin wasn't annoying, you could do this");
    // here we set the behavior of PolKitResul = Auth
    bt->setAuthVisible(true);
    bt->setAuthEnabled(true);
    bt->setAuthText("Play?");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    bt->setAuthToolTip("Only card carrying tweakers can do this!");
    // here we set the behavior of PolKitResul = Yes
    bt->setYesVisible(true);
    bt->setYesEnabled(true);
    bt->setYesText("Play!");
    bt->setYesIcon(QPixmap(":/Icons/custom-yes.png"));
    bt->setYesToolTip("Go ahead, play!");

    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(deletePB, "org.qt.policykit.examples.delete", this);
    bt->setText("Delete!");
    bt->setNoIcon(QPixmap(":/Icons/custom-no.png"));
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    bt->setYesIcon(QPixmap(":/Icons/custom-yes.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(listenPB, "org.qt.policykit.examples.listen", this);
    bt->setText("Listen!");
    bt->setNoIcon(QPixmap(":/Icons/custom-no.png"));
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    bt->setYesIcon(QPixmap(":/Icons/custom-yes.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(setPB, "org.qt.policykit.examples.set", this);
    bt->setText("Set!");
    bt->setNoIcon(QPixmap(":/Icons/custom-no.png"));
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    bt->setYesIcon(QPixmap(":/Icons/custom-yes.png"));
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    bt = new ActionButton(shoutPB, "org.qt.policykit.examples.shout", this);
    bt->setNoIcon(QPixmap(":/Icons/custom-no.png"));
    bt->setNoText("Can't shout!");
    bt->setAuthIcon(QPixmap(":/Icons/action-locked-default.png"));
    bt->setAuthText("Shout?");
    bt->setYesIcon(QPixmap(":/Icons/custom-yes.png"));
    bt->setYesText("Shout!");
    menuActions->addAction(qobject_cast<Action*>(bt));
    toolBar->addAction(qobject_cast<Action*>(bt));
    connect(bt, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
    connect(bt, SIGNAL(clicked(QAbstractButton*, bool)), bt, SLOT(activate()));
    connect(bt, SIGNAL(activated()), this, SLOT(actionActivated()));

    // test configChanged
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
    action->activate();
}

void PkExample::actionActivated()
{
    // This slot is called whenever an action is allowed
    // here you will do your dirt job by calling a helper application
    // that might erase your hardrive ;)
    Action *action = qobject_cast<Action*>(sender());

    // this is our Special Action that after allowed will call the helper
    if (action->is("org.qt.policykit.examples.set")) {
        qDebug() << "toggled for: org.qt.policykit.examples.set";

        QDBusMessage message;
        message = QDBusMessage::createMethodCall("org.qt.policykit.examples",
                  "/",
                  "org.qt.policykit.examples",
                  QLatin1String("set"));
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(setCB->currentText());
        message.setArguments(argumentList);
        // notice the systemBus here..
        QDBusMessage reply = QDBusConnection::systemBus().call(message);
        if (reply.type() == QDBusMessage::ReplyMessage
                && reply.arguments().size() == 1) {
            // the reply can be anything, here we receive a bool
            QListWidgetItem *item;
            if (reply.arguments().first().toBool())
                item = new QListWidgetItem(QPixmap(":/Icons/custom-yes.png"),
                        QString("Implicit authorization for shout has been set to %0")
                        .arg(setCB->currentText()));
            else
                item = new QListWidgetItem(QPixmap(":/Icons/custom-no.png"),
                        QString("Can't change the implicit authorization. Denied."));
            actionList->addItem(item);
            qDebug() << reply.arguments().first().toString();
        } else if (reply.type() == QDBusMessage::MethodCallMessage) {
            qWarning() << "Message did not receive a reply (timeout by message bus)";
        }
        return;
    }

    // As debug message says we are pretending to be the mechanism for the
    // following action, here you will actually call your DBus helper that
    // will run as root (setuid is not needed, see DBus docs).
    // In the helper application you will issue checkAuthorizationSync,
    // passing the action id and the caller pid (which DBus will tell you).
    qDebug() << "pretending to be the mechanism for action:" << action->actionId();

    Authority::Result result;
    UnixProcessSubject *subject;
    
    subject = new UnixProcessSubject(static_cast<uint>(QCoreApplication::applicationPid()));
    
    result = Authority::instance()->checkAuthorizationSync(action->actionId(), subject,
                      Authority::AllowUserInteraction);
    if (result == Authority::Yes) {
        // in the helper you will do the action
        qDebug() << "caller is authorized to do:" << action->actionId();
        QListWidgetItem *item = new QListWidgetItem(QPixmap(":/Icons/custom-yes.png"), action->actionId());
        actionList->addItem(item);
    } else {
        // OR return false to notify the caller that the action is not authorized.
        qDebug() << "caller is NOT authorized to do:" << action->actionId();
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/Icons/custom-no.png"), action->actionId());
        actionList->addItem(item);
    }
}
