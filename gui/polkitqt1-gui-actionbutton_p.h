/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_GUI_ACTIONBUTTON_P_H
#define POLKITQT1_GUI_ACTIONBUTTON_P_H

#include <polkitqt1-gui-actionbutton.h>

#include <QList>
#include <QAbstractButton>

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
