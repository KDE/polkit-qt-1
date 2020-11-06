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

#ifndef POLKITQT1_DETAILS_P_H
#define POLKITQT1_DETAILS_P_H

#include <polkitqt1-details.h>

#include <QSharedData>

#include <polkit/polkit.h>

/**
  * \internal
  */
class PolkitQt1::Details::Data : public QSharedData
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

#endif // DETAILS_P_H
