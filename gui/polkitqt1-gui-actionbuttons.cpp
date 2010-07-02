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

#include "polkitqt1-gui-actionbuttons.h"

#include "polkitqt1-gui-actionbuttons_p.h"

namespace PolkitQt1
{

namespace Gui
{

ActionButtons::ActionButtons(const QList<QAbstractButton *> &buttons, const QString &actionId, QObject *parent)
        : ActionButton(*new ActionButtonsPrivate(buttons), actionId, parent)
{
    setButtons(buttons);
}

ActionButtons::~ActionButtons()
{
}

void ActionButtons::setButtons(const QList<QAbstractButton *> &buttons)
{
    Q_FOREACH(QAbstractButton *ent, buttons) {
        addButton(ent);
    }
}

QList<QAbstractButton *> ActionButtons::buttons() const
{
    Q_D(const ActionButtons);

    return d->buttons;
}

void ActionButtons::addButton(QAbstractButton *button)
{
    Q_D(ActionButtons);

    d->addButton(button);
}

void ActionButtons::removeButton(QAbstractButton *button)
{
    Q_D(ActionButtons);

    d->removeButton(button);
}

}

}

#include "polkitqt1-gui-actionbuttons.moc"
