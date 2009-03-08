/*
 * This file is part of the Polkit-qt project
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

#ifndef ACTIONBUTTONS_H
#define ACTIONBUTTONS_H

#include "actionbutton.h"

namespace PolkitQt
{

class ActionButtonsPrivate;
/**
 * \class ActionButtons actionbuttons.h ActionButtons
 * \author Dario Freddi <drf54321@gmail.com>
 *
 * \brief Class used to hold and update a list of QAbstractButtons
 *
 * This class is a convenience wrapper around ActionButton that lets
 * you associate an undefined number of QAbstractButtons with a single
 * action. Every button will be updated accordingly upon action's properties
 * changes.
 *
 * \see ActionButton
 */
class ActionButtons : public ActionButton
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionButtons)

    public:
        /**
         * Constructs a new ActionButton. You need to pass this
         * constructor an existing list of QAbstractButtons, whose properties
         * will be modified according to the underlying Action
         * object. As ActionButtons inherits from Action, you can
         * define your buttons' behavior right through this wrapper.
         *
         * \see Action
         *
         * \param button the QAbstractButton to associate to this ActionButton
         * \param actionId the action Id to create the underlying Action
         * \param parent the parent object
         */
        ActionButtons(const QList<QAbstractButton*> &buttons, const QString &actionId, QObject *parent = 0);
        virtual ~ActionButtons();

        /**
         * Sets a list of buttons associated to the underlying action.
         *
         * \note If you are calling this function, you're probably
         *       changing the buttons list the action is referring to. If this
         *       is the case, please note that Polkit-Qt does not handle
         *       the previous buttons' memory, so you should take care of
         *       deleting them yourself (if needed). You can retrieve it by
         *       using buttons()
         *
         * \see buttons
         *
         * \param buttons the new buttons associated with the underlying action
         */
        void setButtons(const QList<QAbstractButton*> &buttons);

        /**
         * Returns the current buttons list
         *
         * \return the buttons currently associated with the underlying action
         */
        QList<QAbstractButton*> buttons() const;

        /**
         * Adds a button to the current button list. The button's properties
         * will be updated according to the action upon adding.
         *
         * \param button the button to add
         */
        void addButton(QAbstractButton *button);

        /**
         * Removes a button from the current list. Please note that Polkit-Qt
         * does not handle the removed button's memory, so you should take care of
         * deleting it yourself (if needed).
         *
         * \param button the button to remove
         */
        void removeButton(QAbstractButton *button);
};

}

#endif /* ACTIONBUTTONS_H */
