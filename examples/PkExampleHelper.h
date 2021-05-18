// This is an example not a library
/*
    SPDX-FileCopyrightText: 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PKEXAMPLE_HELPER_H
#define PKEXAMPLE_HELPER_H

#include <QDBusContext>
#include <QCoreApplication>

class PkExampleHelper : public QCoreApplication, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.qt.policykit.examples")
public:
    PkExampleHelper(int &argc, char **argv);
    ~PkExampleHelper() override;

public Q_SLOTS:
    bool set(const QString &action);
    bool setValue(const QString &action);
};


#endif
