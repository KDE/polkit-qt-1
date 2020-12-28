/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
