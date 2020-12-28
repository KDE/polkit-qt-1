// This is an example not a library
/*
    SPDX-FileCopyrightText: 2009 Daniel Nicoletti <dantti85-pk@yahoo.com.br>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>

#include "PkExample.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PkExample example;
    example.show();
    return app.exec();
}
