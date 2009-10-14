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

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <polkit/polkit.h>

using namespace PolkitQt;

Details::Details(QObject *parent) : QObject(parent)
{
    g_type_init();
    m_polkitDetails = polkit_details_new();
}

Details::Details(PolkitDetails *pkDetails, QObject *parent) : QObject(parent)
{
    g_type_init();
    m_polkitDetails = pkDetails;
}

Details::~Details()
{
    g_object_unref(m_polkitDetails);
}

QString Details::lookup(const QString &key) const
{
    const gchar *result = polkit_details_lookup(m_polkitDetails, key.toUtf8().data());
    if (result != NULL)
        return QString::fromUtf8(result);
    else
        return QString();
}

void Details::insert(const QString &key, const QString &value)
{
    polkit_details_insert(m_polkitDetails, key.toUtf8().data(), value.toUtf8().data());
}

QStringList Details::getKeys() const
{
    gchar **result = polkit_details_get_keys(m_polkitDetails);
    QStringList list;
    int len = g_strv_length(result);
    for (int i = 0; i < len; i++)
    {
        list.append(QString::fromUtf8(result[i]));
    }
    g_strfreev(result);
    return list;
}
