/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Dario Freddi <drf@kde.org>
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

#ifndef POLKITQT1_GUI_ACTIONBUTTON_P_H
#define POLKITQT1_GUI_ACTIONBUTTON_P_H

#include <polkitqt1-gui-actionbutton.h>

#include <QtCore/QList>
#include <QtGui/QAbstractButton>

/**
  * \internal
  */
class PolkitQt1::Gui::ActionButtonPrivate
{
public:
    ActionButtonPrivate(const QList<QAbstractButton *> &b)
            : buttons(b) {}
    virtual ~ActionButtonPrivate() {}

    void addButton(QAbstractButton *button);
    void removeButton(QAbstractButton *button);
    void updateButton();
    void streamClicked(bool);

    Q_DECLARE_PUBLIC(ActionButton)
    ActionButton *q_ptr;

    QList<QAbstractButton *> buttons;
};

#endif /* ACTIONBUTTON_P_H */
