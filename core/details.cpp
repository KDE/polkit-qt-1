/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Radek Novacek <rnovacek@redhat.com>
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

#include "details.h"

#include <QtCore/QStringList>

#include <polkit/polkit.h>

namespace PolkitQt1
{

class Details::Private
{
public:
    Private() {}

    PolkitDetails *polkitDetails;
};

Details::Details(QObject *parent)
        : QObject(parent)
        , d(new Private)
{
    g_type_init();
    d->polkitDetails = polkit_details_new();
}

Details::Details(PolkitDetails *pkDetails, QObject *parent)
        : QObject(parent)
        , d(new Private)
{
    g_type_init();
    d->polkitDetails = pkDetails;
}

Details::~Details()
{
    g_object_unref(d->polkitDetails);
    delete d;
}

QString Details::lookup(const QString &key) const
{
    const gchar *result = polkit_details_lookup(d->polkitDetails, key.toUtf8().data());
    if (result != NULL)
        return QString::fromUtf8(result);
    else
        return QString();
}

void Details::insert(const QString &key, const QString &value)
{
    polkit_details_insert(d->polkitDetails, key.toUtf8().data(), value.toUtf8().data());
}

QStringList Details::keys() const
{
    gchar **result = polkit_details_get_keys(d->polkitDetails);
    QStringList list;
    int len = g_strv_length(result);
    for (int i = 0; i < len; i++) {
        list.append(QString::fromUtf8(result[i]));
    }
    g_strfreev(result);
    return list;
}

}

#include "details.moc"
