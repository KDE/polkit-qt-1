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

#include <QtCore>

#include <QtCore/QString>

namespace PolKitQt {

/**
 * \class QPkContext polkit_qt_context.h QPkContext
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
 * Call QPkContext::instance() to get an instance of the QPkContext object
 */
class POLKIT_QT_EXPORT QPkContext : public QObject
{
Q_OBJECT
public:
    /**
     * \brief Returns an instance of QPkContext
     *
     * The QPkContext class is a singleton, you can call this method several times,
     * a single QPkContext object will exist.
     */
    static QPkContext* instance();
    ~QPkContext();

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
     * file is changed or when ConsoleKit report Activities.
     * You should connect to this signal if you want to track
     * actions.
     * \note If you use PkAction you'll probably prefer to
     * use the resultChanged() signal to track Actions changes.
     */
    void configChanged();

private slots:
    void watchActivatedContext(int fd);

private:
    QPkContext(QObject *parent = 0);
    static QPkContext *m_self;
    void init();
    bool m_hasError;
    QString m_lastError;

    QMap<int, QSocketNotifier*> m_watches;

    static int  io_add_watch(PolKitContext *context, int fd);
    static void io_remove_watch(PolKitContext *context, int fd);
    static void pk_config_changed(PolKitContext *context, void *user_data);
};

}

#endif
