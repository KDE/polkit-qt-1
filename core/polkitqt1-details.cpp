/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polkitqt1-details.h"

#include <QStringList>

#include <polkit/polkit.h>

namespace PolkitQt1
{

class Details::Data : public QSharedData
{
public:
    Data() {}
    Data(const Data &other)
        : QSharedData(other)
        , polkitDetails(other.polkitDetails)
    {
        if (polkitDetails != nullptr) {
            g_object_ref(polkitDetails);
        }
    }
    ~Data()
    {
        if (polkitDetails != nullptr) {
            g_object_unref(polkitDetails);
        }
    }

    PolkitDetails *polkitDetails;
};

Details::Details()
        : d(new Data)
{
    g_type_init();
    d->polkitDetails = polkit_details_new();
}

Details::Details(PolkitDetails *pkDetails)
        : d(new Data)
{
    g_type_init();
    d->polkitDetails = pkDetails;
    
    if (d->polkitDetails != nullptr) {
        g_object_ref(d->polkitDetails);
    }
}

Details::~Details()
{
}

Details& Details::operator=(const PolkitQt1::Details& other)
{
    d = other.d;
    return *this;
}

QString Details::lookup(const QString &key) const
{
    const gchar *result = polkit_details_lookup(d->polkitDetails, key.toUtf8().data());
    if (result != nullptr) {
        return QString::fromUtf8(result);
    } else {
        return QString();
    }
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
