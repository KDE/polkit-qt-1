// This is an example not a library
/***************************************************************************
 *   Copyright (C) 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>        *
 *   Copyright (C) 2009 Radek Novacek    <rnovacek@redhat.com>             *
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

#include "PkExampleHelper.h"
#include "examplesadaptor.h"

#include <Authority>
#include <Auth>

#include <QtDBus/QDBusConnection>
#include <QTimer>
#include <QDebug>
#include <QtXml>

#define MINUTE 60000

using namespace PolkitQt;

PkExampleHelper::PkExampleHelper(int &argc, char **argv)
        : QCoreApplication(argc, argv)
{
    qDebug() << "Creating Helper";
    (void) new ExamplesAdaptor(this);
    // Register the DBus service
    if (!QDBusConnection::systemBus().registerService("org.qt.policykit.examples")) {
        qDebug() << QDBusConnection::systemBus().lastError().message();;
        QTimer::singleShot(0, this, SLOT(quit()));
        return;
    }

    if (!QDBusConnection::systemBus().registerObject("/", this)) {
        qDebug() << "unable to register service interface to dbus";
        QTimer::singleShot(0, this, SLOT(quit()));
        return;
    }
    // Normally you will set a timeout so your application can
    // free some resources of the poor client machine ;)
    QTimer::singleShot(MINUTE, this, SLOT(quit()));
}

PkExampleHelper::~PkExampleHelper()
{
    qDebug() << "Destroying Helper";
}

bool PkExampleHelper::set(const QString &action)
{
    qDebug() << "PkExampleHelper::set" << action;
    // message().service() is the service name of the caller
    // We can check if the caller is authorized to the following action
    Auth::Result result;
    SystemBusName *subject;
    
    subject = new SystemBusName(message().service());
    
    result = Auth::checkAuthorization("org.qt.policykit.examples.set",
                                      subject , Auth::AllowUserInteraction);
    if (result == Auth::Yes) {
        qDebug() << message().service() << QString("Implicit authorization set to") << action;
        // Caller is authorized so we can perform the action
        return setValue(action);
    } else {
        qDebug() << message().service() << QString("Can't set the implicit authorization");
        // Caller is not authorized so the action can't be performed
        return false;
    }
}

bool PkExampleHelper::setValue(const QString &action)
{
    // This action must be authorized first. It will set the implicit
    // authorization for the Shout action by editing the .policy file
    QDomDocument doc = QDomDocument("policy");
    QFile file("/usr/share/polkit-1/actions/org.qt.policykit.examples.policy");
    if (!file.open(QIODevice::ReadOnly))
        return false;
    doc.setContent(&file);
    file.close();
    QDomElement el = doc.firstChildElement("policyconfig").
                         firstChildElement("action");
    while (!el.isNull() && el.attribute("id", QString()) != "org.qt.policykit.examples.shout")
    {
        el = el.nextSiblingElement("action");
    }
    el = el.firstChildElement("defaults");
    el = el.firstChildElement("allow_active");
    if (el.isNull())
        return false;
    el.firstChild().toText().setData(action);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QTextStream stream(&file);
    doc.save(stream, 2);
    file.close();
    return true;
}
