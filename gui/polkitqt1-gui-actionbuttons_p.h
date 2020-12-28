/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_GUI_ACTIONBUTTONS_P_H
#define POLKITQT1_GUI_ACTIONBUTTONS_P_H

#include "polkitqt1-gui-actionbutton_p.h"

/**
  * \internal
  */
class PolkitQt1::Gui::ActionButtonsPrivate : public ActionButtonPrivate
{
public:
    ActionButtonsPrivate(const QList<QAbstractButton *> &b)
            : ActionButtonPrivate(b) {}
};

#endif /* ACTIONBUTTONS_P_H */
