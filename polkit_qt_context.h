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

#ifndef POLKIT_QT_CONTEXT_H
#define POLKIT_QT_CONTEXT_H

#include <polkit-dbus/polkit-dbus.h>

#include <QtCore>

#include <QtCore/QString>

namespace PolKitQt {

/**
 * \class PolkitQtAuth polkit_qt_auth.h PolkitQtAuth
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to obtain authorizations
 *
 * This class is a simple wrapper around the DBus interface
 * org.freedesktop.PolicyKit.AuthenticationAgent to make usage of
 * policykit easier in Qt/KDE worlds
 *
 * \note This class is a singleton, its constructor is private. Call PkContext::instance() to get
 * an instance of the PkContext object
 */
class PkContext : public QObject
{
Q_OBJECT
public:
    /**
     * \brief Returns an instance of PkContext
     *
     * The PkContext class is a singleton, you can call this method several times,
     * a single PkContext object will exist.
     */
    static PkContext* instance();
    ~PkContext();

    PolKitContext *pkContext;
    PolKitTracker *pkTracker;

signals:
    void configChanged();

private slots:
    void watchActivatedContext(int fd);

private:
    PkContext(QObject *parent = 0);
    static PkContext *m_self;

    QMap<int, QSocketNotifier*> m_watches;

    static int  io_add_watch(PolKitContext *context, int fd);
    static void io_remove_watch(PolKitContext *context, int fd);
    static void pk_config_changed(PolKitContext *ontext, void *user_data);
};

}

#endif
