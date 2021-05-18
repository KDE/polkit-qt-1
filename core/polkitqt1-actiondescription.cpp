/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>
    SPDX-FileCopyrightText: 2010 Dario Freddi <drf@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polkitqt1-actiondescription.h"

#include <QString>

#include <polkit/polkit.h>

namespace PolkitQt1
{

class ActionDescription::Data : public QSharedData
{
public:
    Data() {}
    Data(const Data& other)
        : QSharedData(other)
        , actionId(other.actionId)
        , description(other.description)
        , message(other.message)
        , vendorName(other.vendorName)
        , vendorUrl(other.vendorUrl)
        , iconName(other.iconName)
        , implicitAny(other.implicitAny)
        , implicitInactive(other.implicitInactive)
        , implicitActive(other.implicitActive)
    {
    }
    virtual ~Data() {}

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

ActionDescription::ActionDescription()
        : d(new Data)
{

}

ActionDescription::ActionDescription(PolkitActionDescription *polkitActionDescription)
        : d(new Data)
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

ActionDescription::ActionDescription(const PolkitQt1::ActionDescription& other)
        : d(other.d)
{
}

ActionDescription& ActionDescription::operator=(const PolkitQt1::ActionDescription& other)
{
    d = other.d;
    return *this;
}

ActionDescription::~ActionDescription()
{
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

}
