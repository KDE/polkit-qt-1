/***************************************************************************
 *   Copyright (C) 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "PkExample.h"

#include <QDebug>

using namespace PolkitQt;

PkExample::PkExample(QMainWindow *parent)
        : QMainWindow(parent)
{
    setupUi(this);

    cryA = new Action("org.qt.policykit.examples.cry", this);
    cryA->setText("Cry!");
    menuActions->addAction(cryA);
    toolBar->addAction(cryA);
    connect(cryA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));

    playA = new Action("org.qt.policykit.examples.play", this);
    playA->setText("Play!");
    menuActions->addAction(playA);
    toolBar->addAction(playA);
    connect(playA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));

    kickA = new Action("org.qt.policykit.examples.kick", this);
    kickA->setText("Kick... (long)");
    kickA->setNoVisible(true);
    kickA->setNoEnabled(true);
    kickA->setNoText("Kick (long)");
    kickA->setNoToolTip("If your admin wasn't annoying, you could do this");

    kickA->setAuthVisible(true);
    kickA->setAuthEnabled(true);
    kickA->setAuthText("Kick... (long)");
    kickA->setAuthToolTip("Only card carrying tweakers can do this!");

    kickA->setYesVisible(true);
    kickA->setYesEnabled(true);
    kickA->setYesText("Kick! (long)");
    kickA->setYesToolTip("Go ahead, kick kick kick!");

    menuActions->addAction(kickA);
    toolBar->addAction(kickA);
    connect(kickA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));

    deleteA = new Action("org.qt.policykit.examples.delete", this);
    deleteA->setText("Delete!");
    menuActions->addAction(deleteA);
    toolBar->addAction(deleteA);
    connect(deleteA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));

    bleedA = new Action("org.qt.policykit.examples.bleed", this);
    bleedA->setText("Bleed!");
    menuActions->addAction(bleedA);
    toolBar->addAction(bleedA);
    connect(bleedA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));

    blowUpA = new Action("org.qt.policykit.examples.blow-up", this);
    blowUpA->setText("Click to make changes...");
    menuActions->addAction(blowUpA);
    toolBar->addAction(blowUpA);
    connect(blowUpA, SIGNAL(triggered(bool)), this, SLOT(activateAction()));
}

PkExample::~PkExample()
{
}

void PkExample::activateAction()
{
    Action *action = (Action *) sender();
    action->activate(winId());
}