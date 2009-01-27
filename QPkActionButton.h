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

#ifndef POLKIT_QT_ACTION_BUTTON_H
#define POLKIT_QT_ACTION_BUTTON_H

#include "polkit_qt_export.h"
#include "polkit_qt_action.h"

#include <QtCore/QString>
#include <QtGui/QAbstractButton>

namespace QPolicyKit {

/**
 * \class ActionButton polkit_qt_auth.h ActionButton
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 *
 * \brief Class used to hold and update a QAbstractButton
 *
 * This class allows you to associate QAbstractButtons
 * (ie QPushButton) to a PolicyKit Action. It will update the
 * button properties according to the PolicyKit Action on itself.
 *
 * \note You should connect the activated() signal to receive
 * a notification when the user clicked the button and he's
 * permitted to do the given action. If you set 'noEnabled'
 * to TRUE it will be emitted when PolKitResult is NO.
 */
class POLKIT_QT_EXPORT ActionButton : public Action
{
Q_OBJECT
public:
    ActionButton(QAbstractButton *button, const QString &actionId, QWidget *parent = 0);

private slots:
    void updateButton();
    void toggled();
    void activateProxy();

private:
    class Private;
    Private *d;
};

}

#endif
