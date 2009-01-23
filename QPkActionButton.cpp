/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
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

#include "QPkActionButton.h"

using namespace PolKitQt;

QPkActionButton::QPkActionButton(QAbstractButton *button, const QString &actionId, WId winId, QWidget *parent)
 : QPkAction(actionId, winId, parent), m_button(button)
{
    updateButton();
    connect(this, SIGNAL(dataChanged()), SLOT(updateButton()));
    connect(m_button, SIGNAL(clicked()), SLOT(activate()));
}

void QPkActionButton::updateButton()
{
    qDebug() << "QPkActionButton::updateButton()";
    m_button->setVisible(visible());
    m_button->setEnabled(enabled());
    // We check for Null to see if the user
    // never set anything as "" is not NULL
    if (!text().isNull()) {
        m_button->setText(text());
    }
    if (!toolTip().isNull()) {
        m_button->setToolTip(toolTip());
    }
    if (!whatsThis().isNull()) {
        m_button->setWhatsThis(whatsThis());
    }
    if (!icon().isNull()) {
        m_button->setIcon(icon());
    }
}

#include "QPkActionButton.moc"
