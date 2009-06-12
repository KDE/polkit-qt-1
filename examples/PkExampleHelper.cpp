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

#include "PkExampleHelper.h"
#include "examplesadaptor.h"

#include <Context>
#include <Auth>

#include <QtDBus/QDBusConnection>
#include <QTimer>
#include <QDebug>

#define MINUTE 60000

using namespace PolkitQt;

PkExampleHelper::PkExampleHelper(int &argc, char **argv)
        : QCoreApplication(argc, argv)
{
    qDebug() << "Creating Helper";
    (void) new ExamplesAdaptor(this);
    if (!QDBusConnection::systemBus().registerService("org.qt.policykit.examples")) {
        qDebug() << "another helper is already running";
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

const QString PkExampleHelper::play(const QString &user)
{
    qDebug() << "PkExampleHelper::play" << user;
    // here you need to notice two important things:
    // 1st message().service() is the service name of the caller
    //     with it we can check if the caller is authorized to
    //     do the following action
    // 2nd the "true" parameter, this is REALLY important, you MUST
    //     always set it to true if you are in the helper. This way
    //     one shot actions can be properly revoked, use "true" even
    //     if your action aren't one shot, since they can easyly changed
    //     by any PolicyKit Authorization application.
    Auth::Result result;
    result = Auth::isCallerAuthorized("org.qt.policykit.examples.play",
                                      message().service(),
                                      true);
    if (result == Auth::Yes) {
        qDebug() << message().service() << QString(user + " can play");
        return QString(user + " can play");
    } else {
        qDebug() << message().service() << QString("Sorry " + user + " can not play");
        return QString("Sorry " + user + " can not play");
    }
}
