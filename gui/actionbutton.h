/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
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

#ifndef POLKIT_QT_ACTIONBUTTON_H
#define POLKIT_QT_ACTIONBUTTON_H

#include "export.h"
#include "action.h"

#include <QtGui/QAbstractButton>

namespace PolkitQt
{

class ActionButtonPrivate;
/**
 * \class ActionButton actionbutton.h ActionButton
 * \author Daniel Nicoletti <dantti85-pk@yahoo.com.br>
 * \author Dario Freddi <drf@kde.org>
 *
 * \brief Class used to hold and update a QAbstractButton
 *
 * This class allows you to associate QAbstractButtons
 * (i.e. QPushButton) to a PolicyKit Action. It will update the
 * button properties according to the PolicyKit Action automatically.
 *
 * \note You should connect the activated() signal to receive
 * a notification when the user clicked the button and gets
 * permission to perform the given action. If you set 'noEnabled'
 * to \c true it will be emitted when PolKitResult is NO.
 */
class POLKIT_QT_EXPORT ActionButton : public Action
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionButton)
    Q_DISABLE_COPY(ActionButton)

public:
    /**
     * Constructs a new ActionButton. You need to pass this
     * constructor an existing QAbstractButton, whose properties
     * will be modified according to the underlying Action
     * object. As ActionButton inherits from Action, you can
     * define your button's behavior right through this wrapper.
     *
     * \see Action
     *
     * \param button the QAbstractButton to associate to this ActionButton
     * \param actionId the action Id to create the underlying Action
     * \param parent the parent object
     */
    explicit ActionButton(QAbstractButton *button, const QString &actionId = QString(), QObject *parent = 0);
    virtual ~ActionButton();

    /**
     * Sets the button associated to the underlying action.
     *
     * \note If you are calling this function, you're probably
     *       changing the button the action is referring to. If this
     *       is the case, please note that Polkit-Qt does not handle
     *       the previous button's memory, so you should take care of
     *       deleting it yourself (if needed). You can retrieve it by
     *       using button()
     *
     * \see button
     *
     * \param button the new button associated with the underlying action
     */
    void setButton(QAbstractButton *button);

    /**
     * Returns the current button
     *
     * \return the button currently associated with the underlying action
     */
    QAbstractButton *button() const;

public Q_SLOTS:
    /**
     * Connect clicked() signals to this slot. This should be
     * manually done, as in some cases we might want
     * to manually call this. Calling this will emit activated().
     *
     * \note This slot is reentrant which is likely to only be a problem
     * if you are creating an interface to setup PolicyKit policies.
     * \note If you have a checkbox, connect to its' clicked() signal
     * to avoid an infinite loop as this function internally calls setChecked().
     * You can always use the clicked(bool) signal in this class to
     * connect to here.
     * \warning if you use this class take care to not call Action::activate
     * otherwise your checkable buttons won't be properly updated.
     */
    bool activate();

Q_SIGNALS:
    /**
     * Emitted when the abstract button clicked(bool) signal
     * is emitted. This allows you to use qobject_cast<ActionButton*>(sender())
     * in a slot connected to this signal and call activate() on it.
     *
     * \note you will normally want to connect this signal
     * to the activate slot.
     *
     * \param button the button that has been clicked
     * \param checked the checked state, if applicable. Otherwise \c false
     *
     */
    void clicked(QAbstractButton *button, bool checked = false);

protected:
    ActionButton(ActionButtonPrivate &dd, const QString &actionId, QObject *parent = 0);

    ActionButtonPrivate * d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void updateButton())
    Q_PRIVATE_SLOT(d_func(), void streamClicked(bool))
};


}

#endif
