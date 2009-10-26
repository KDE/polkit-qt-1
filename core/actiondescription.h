/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
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

#ifndef POLKIT_QT_ACTION_DESCRIPTION_H
#define POLKIT_QT_ACTION_DESCRIPTION_H

#include "export.h"

#include <QtCore/QList>
#include <QtCore/QMetaType>

typedef struct _PolkitActionDescription PolkitActionDescription;

namespace PolkitQt
{
class ActionDescription;
typedef QList<ActionDescription*> ActionDescriptionList;

/**
 * \class ActionDescription actiondescription.h ActionDescription
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief Class used to encapsulate a registered action.
 */
class POLKIT_QT_EXPORT ActionDescription
{
public:
    enum ImplicitAuthorization {
	/** Unknown whether the subject is authorized, never returned in any public API. **/
	Unknown = -1,
	/** Subject is not authorized. **/
	NotAuthorized = 0,
	/** Authentication is required. **/
	AuthenticationRequired = 1,
	/** Authentication as an administrator is required. **/
	AdministratorAuthenticationRequired = 2,
	/** Authentication is required. If the authorization is obtained, it is retained. **/
	AuthenticationRequiredRetained = 3,
	/** Authentication as an administrator is required. If the authorization is obtained, it is retained. **/ 
	AdministratorAuthenticationRequiredRetained = 4,
	/** The subject is authorized. **/
	Authorized = 5
    };
    
    ActionDescription(PolkitActionDescription *actionDescription);    
    ~ActionDescription();
    
    QString actionId() const;
    QString description() const;
    QString message() const;
    QString vendorName() const;
    QString vendorUrl() const;
    QString iconName() const;
    
    ActionDescription::ImplicitAuthorization implicitAny() const;
    ActionDescription::ImplicitAuthorization implicitInactive() const;
    ActionDescription::ImplicitAuthorization implicitActive() const;
private:
    class Private;
    Private * const d;
};
}

Q_DECLARE_METATYPE(PolkitQt::ActionDescriptionList);

#endif //POLKIT_QT_ACTION_DESCRIPTION_H
