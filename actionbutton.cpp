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

#include "actionbutton_p.h"

using namespace PolkitQt;

ActionButton::ActionButton(QAbstractButton *button, const QString &actionId, QObject *parent)
        : Action(actionId, parent)
        , d_ptr(new ActionButtonPrivate(QList<QAbstractButton*>() << button))
{
    d_ptr->q_ptr = this;

    setButton(button);
    connect(this, SIGNAL(dataChanged()), SLOT(updateButton()));
}

ActionButton::ActionButton(ActionButtonPrivate &dd, const QString &actionId, QObject *parent)
        : Action(actionId, parent)
        , d_ptr(&dd)
{
    d_ptr->q_ptr = this;

    connect(this, SIGNAL(dataChanged()), SLOT(updateButton()));
}

void ActionButtonPrivate::updateButton()
{
    Q_Q(ActionButton);

    foreach (QAbstractButton *ent, buttons) {
        ent->setVisible(q->isVisible());
        ent->setEnabled(q->isEnabled());
        ent->setText(q->text());
        if (!q->toolTip().isNull()) {
            ent->setToolTip(q->toolTip());
        }
        if (!q->whatsThis().isNull()) {
            ent->setWhatsThis(q->whatsThis());
        }
        ent->setIcon(q->icon());
        // if the item cannot do the action anymore
        // lets revert to the initial state
        if (ent->isCheckable()) {
            ent->setChecked(q->isChecked());
        }
    }
}

bool ActionButton::activate()
{
    Q_D(ActionButton);

    bool tg = false;
    foreach (QAbstractButton *ent, d->buttons) {
        if (ent->isCheckable()) {
            // we set the the current Action state
            ent->setChecked(isChecked());
            // toggle the action cause we are not directly connected there..
            tg = true;
        }
    }

    if (tg) {
        toggle();
    }

    return Action::activate(d->buttons.first()->winId());
}

void ActionButton::setButton(QAbstractButton *button)
{
    Q_D(ActionButton);

    // First, let's clear the list
    foreach (QAbstractButton *ent, d->buttons) {
        d->removeButton(ent);
    }

    // And then add it
    d->addButton(button);
}

void ActionButtonPrivate::addButton(QAbstractButton *button)
{
    Q_Q(ActionButton);

    buttons.append(button);
    QObject::connect(button, SIGNAL(clicked(bool)), q, SLOT(streamClicked(bool)));
    QObject::connect(q, SIGNAL(toggled(bool)), button, SLOT(toggle()));
    if (q->isCheckable()) {
        // the button should follow our first buttons
        button->setCheckable(true);
    } else if (button->isCheckable()) {
        // if we are not checkable BUT the button
        // is (eg a QCheckBox) we should set all buttons to
        // checkable.
        foreach (QAbstractButton *ent, buttons) {
            ent->setCheckable(true);
        }
        // set the checkable state of Action to store the initial state
        q->setCheckable(true);
    }
    // call this after m_activateOnCheck receives the value
    updateButton();
}

void ActionButtonPrivate::removeButton(QAbstractButton *button)
{
    Q_Q(ActionButton);

    if (buttons.contains(button)) {
        QObject::disconnect(button, SIGNAL(clicked(bool)), q, SLOT(streamClicked(bool)));
        QObject::disconnect(q, SIGNAL(toggled(bool)), button, SLOT(toggle()));
        buttons.removeOne(button);
    }
}

QAbstractButton *ActionButton::button() const
{
    Q_D(const ActionButton);

    return d->buttons.first();
}

void ActionButtonPrivate::streamClicked(bool c)
{
    Q_Q(ActionButton);

    emit q->clicked(qobject_cast<QAbstractButton*>(q->sender()), c);
}

#include "moc_actionbutton.cpp"
