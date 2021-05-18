// This is an example not a library
/*
    SPDX-FileCopyrightText: 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PKEXAMPLE_H
#define PKEXAMPLE_H

#include "ui_PkExample.h"

class PkExample : public QMainWindow, Ui::PkExample
{
    Q_OBJECT

public:
    PkExample(QMainWindow *parent = nullptr);
    ~PkExample();

private Q_SLOTS:
    void activateAction();
    void actionActivated();
};


#endif
