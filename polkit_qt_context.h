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

#include "polkit_qt_export.h"

#include <polkit-dbus/polkit-dbus.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtCore/QMap>
#include <QtXml/QDomDocument>

class QSocketNotifier;

namespace QPolicyKit {

/**
 * \class Context polkit_qt_context.h Context
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Convenience class for Qt/KDE aplications
 *
 * This class is a singleton that provides easy of use
 * for PolKitContext and PolKitTracker, it emits configChanged()
 * whether PolicyKit files change (e.g. the PolicyKit.conf
 * or .policy files) or when ConsoleKit report activities changes.
 *
 * \note This class is a singleton, its constructor is private.
 * Call Context::instance() to get an instance of the Context object
 */
class POLKIT_QT_EXPORT Context : public QObject
{
Q_OBJECT
public:
    /**
     * \brief Returns an instance of Context
     *
     * The Context class is a singleton, you can call this method several times,
     * a single Context object will exist.
     */
    static Context* instance();

    Context(QObject *parent = 0);
    ~Context();

    /**
     * You should always call first this method,
     * if an error exist it'll try to reinitialize
     * \return TRUE if not error has happened
     */
    bool hasError();

    /**
     * \return the last error message
     */
    QString lastError() const;

    /**
     * Do not use any of the life cycle methods of these objects;
     * only use them to gather information.
     */
    PolKitContext *pkContext;
    PolKitTracker *pkTracker;

signals:
    /**
     * This signal will be emitted when some configuration
     * file is changed (e.g. /etc/PolicyKit/PolicyKit.conf or
     * .policy files).
     * You should connect to this signal if you want to track
     * actions.
     * \note If you use PkAction you'll probably prefer to
     * use the resultChanged() signal to track Actions changes.
     */
    void configChanged();

    /**
     * This signal is emitted when CconsoleKit configuration
     * chages, this might happen when a session becomes active
     * or inactive.
     * actions.
     * If you want to track you actions directly you should
     * connect to this signal as this might change the result
     * PolicyKit will return.
     * \note If you use PkAction you'll probably prefer to
     * use the resultChanged() signal to track Actions changes.
     */
    void consoleKitDBChanged();

private slots:
    void watchActivatedContext(int fd);
    void dbusFilter(const QDBusMessage &message);

private:
    void init();
    bool m_hasError;
    QString m_lastError;
    DBusConnection *m_systemBus;

    QMap<int, QSocketNotifier*> m_watches;

    static int  io_add_watch(PolKitContext *context, int fd);
    static void io_remove_watch(PolKitContext *context, int fd);
    static void pk_config_changed(PolKitContext *context, void *user_data);

    QDomDocument introspect(const QString &service, const QString &path, const QDBusConnection &c);
    QStringList getSignals(const QDomDocument &iface);
};

}

#endif
