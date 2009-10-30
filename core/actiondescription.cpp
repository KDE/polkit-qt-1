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

#include "actiondescription.h"

#include <QtCore/QString>

#include <polkit/polkit.h>

using namespace PolkitQt;

class ActionDescription::Private
{
    public:
        Private() {}

        QString actionId;
        QString description;
        QString message;
        QString vendorName;
        QString vendorUrl;
        QString iconName;
    
        ActionDescription::ImplicitAuthorization implicitAny;
        ActionDescription::ImplicitAuthorization implicitInactive;
        ActionDescription::ImplicitAuthorization implicitActive;
};

ActionDescription::ActionDescription(PolkitActionDescription *polkitActionDescription)
        : d(new Private)
{
    g_type_init();
    
    d->actionId = QString::fromUtf8(polkit_action_description_get_action_id(polkitActionDescription));
    d->description = QString::fromUtf8(polkit_action_description_get_description(polkitActionDescription));
    d->message = QString::fromUtf8(polkit_action_description_get_message(polkitActionDescription));
    d->vendorName = QString::fromUtf8(polkit_action_description_get_vendor_name(polkitActionDescription));
    d->vendorUrl = QString::fromUtf8(polkit_action_description_get_vendor_url(polkitActionDescription));
    d->iconName = QString::fromUtf8(polkit_action_description_get_icon_name(polkitActionDescription));
    
    d->implicitAny = static_cast<ActionDescription::ImplicitAuthorization>(polkit_action_description_get_implicit_any(
	polkitActionDescription));
    d->implicitInactive = static_cast<ActionDescription::ImplicitAuthorization>(polkit_action_description_get_implicit_inactive(
	polkitActionDescription));
    d->implicitActive = static_cast<ActionDescription::ImplicitAuthorization>(polkit_action_description_get_implicit_active(
	polkitActionDescription)); 
}

ActionDescription::~ActionDescription()
{
    delete d;
}

QString ActionDescription::actionId() const
{
    return d->actionId;
}

QString ActionDescription::description() const
{
    return d->description;
}

QString ActionDescription::message() const
{
    return d->message;
}

QString ActionDescription::vendorName() const
{
    return d->vendorName;
}

QString ActionDescription::vendorUrl() const
{
    return d->vendorUrl;
}

QString ActionDescription::iconName() const
{
    return d->iconName;
}

ActionDescription::ImplicitAuthorization ActionDescription::implicitAny() const
{
    return d->implicitAny;
}

ActionDescription::ImplicitAuthorization ActionDescription::implicitInactive() const
{
    return d->implicitInactive;
}

ActionDescription::ImplicitAuthorization ActionDescription::implicitActive() const
{
    return d->implicitActive;
}
