/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * Copyright (C) 2009 Dario Freddi <drf54321@gmail.com>
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

#include "export.h"

#include <polkit-dbus/polkit-dbus.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtCore/QMap>
#include <QtXml/QDomDocument>

class QSocketNotifier;


/**
 * \namespace PolkitQt PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt
{

/**
 * \class Context context.h Context
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf54321@gmail.com>
 *
 * \brief Convenience class for Qt/KDE aplications
 *
 * This class is a singleton that provides makes easy the usage
 * of PolKitContext and PolKitTracker. It emits configChanged()
 * whenever PolicyKit files change (e.g. the PolicyKit.conf
 * or .policy files) or when ConsoleKit reports activities changes.
 *
 * \note This class is a singleton, its constructor is private.
 * Call Context::instance() to get an instance of the Context object.
 * Do not delete Context::instance(), cleanup will be done automatically.
 */
class POLKIT_QT_EXPORT Context : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Context)
public:
    /**
     * \brief Returns the instance of Context
     *
     * Returns the current instance of Context. Call this function whenever
     * you need to access the Context class.
     *
     * \note Context is a singleton. Memory is handled by polkit-qt, so you just
     * need to call this function to get a working instance of Context.
     * Don't delete the object after having used it.
     *
     * \return The current context instance
     */
    static Context* instance();

    ~Context();

    /**
     * You should always call this method first, since if an error is detected,
     * the library will retry to initialize itself.
     *
     * \return true if the library is ready, false if an error occurred
     */
    bool hasError() const;

    /**
     * \return the last error message
     */
    QString lastError() const;

    /**
     * Returns the current instance of PolKitContext. If you are handling
     * it through Polkit-qt (which is quite likely, since you are calling
     * this function), DO NOT use any PolicyKit API's specific method that
     * modifies the instance on it, unless you're completely aware of what you're doing and
     * of the possible consequencies. Use this instance only to gather information.
     *
     * \return the current PolKitContext instance
     */
    PolKitContext *getPolKitContext();

    /**
     * Returns the current instance of PolKitTracker. If you are handling
     * it through Polkit-qt (which is quite likely, since you are calling
     * this function), DO NOT use any PolicyKit API's specific method that
     * modifies the instance on it, unless you're completely aware of what you're doing and
     * of the possible consequencies. Use this instance only to gather information.
     *
     * \return the current PolKitTracker instance
     */
    PolKitTracker *getPolKitTracker();

Q_SIGNALS:
    /**
     * This signal will be emitted when a configuration
     * file gets changed (e.g. /etc/PolicyKit/PolicyKit.conf or
     * .policy files).
     * Connect to this signal if you want to track down
     * actions.
     *
     * \note If you use Action you'll probably prefer to
     * use the dataChanged() signal to track Actions changes.
     */
    void configChanged();

    /**
     * This signal is emitted when ConsoleKit configuration
     * changes. This might happen when a session becomes active
     * or inactive.
     *
     * If you want to track your actions directly you should
     * connect to this signal, as this might change the return value
     * PolicyKit will give you.
     *
     * \note If you use Action you'll probably prefer to
     * use the dataChanged() signal to track Action changes.
     */
    void consoleKitDBChanged();

private:
    Context(QObject *parent = 0);

    class Private;
    friend class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void watchActivatedContext(int fd))
    Q_PRIVATE_SLOT(d, void dbusFilter(const QDBusMessage &message))
};

}

#endif
