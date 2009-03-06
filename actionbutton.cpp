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

#include "actionbutton.h"

using namespace PolkitQt;

class ActionButton::Private
{
public:
    Private(QAbstractButton *b)
            : button(b) {};

    QAbstractButton *button;
};

ActionButton::ActionButton(QAbstractButton *button, const QString &actionId, QObject *parent)
        : Action(actionId, parent)
        , d(new Private(button))
{
    setButton(button);
    connect(this, SIGNAL(dataChanged()), SLOT(updateButton()));
}

void ActionButton::updateButton()
{
    d->button->setVisible(isVisible());
    d->button->setEnabled(isEnabled());
    // We check for Null to see if the user
    // never set anything as "" is not NULL
    if (!text().isNull()) {
        d->button->setText(text());
    }
    if (!toolTip().isNull()) {
        d->button->setToolTip(toolTip());
    }
    if (!whatsThis().isNull()) {
        d->button->setWhatsThis(whatsThis());
    }
    if (!icon().isNull()) {
        d->button->setIcon(icon());
    }
    // if the item cannot do the action anymore
    // lets revert to the initial state
    if (d->button->isCheckable()) {
        d->button->setChecked(isChecked());
    }
}

bool ActionButton::activate()
{
    if (d->button->isCheckable()) {
        // we set the the current Action state
        d->button->setChecked(isChecked());
        // toggle the action cause we are not directly connected there..
        toggle();
    }
    return Action::activate(d->button->winId());
}

void ActionButton::setButton(QAbstractButton *button)
{
    if (d->button) {
        disconnect(d->button, SIGNAL(clicked(bool)), this, SIGNAL(clicked(bool)));
        disconnect(this, SIGNAL(toggled(bool)), d->button, SLOT(toggle()));
    }
    d->button = button;
    connect(d->button, SIGNAL(clicked(bool)), this, SIGNAL(clicked(bool)));
    // set the checkable state of Action to store the initial state
    setCheckable(d->button->isCheckable());
    if (d->button->isCheckable()) {
        // store the checked state in Action
        setChecked(d->button->isChecked());
        connect(this, SIGNAL(toggled(bool)), d->button, SLOT(toggle()));
    }
    // call this after m_activateOnCheck receives the value
    updateButton();
}

QAbstractButton *ActionButton::button()
{
    return d->button;
}
