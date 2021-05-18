// This is an example not a library
/*
    SPDX-FileCopyrightText: 2008 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
    SPDX-FileCopyrightText: 2009 Radek Novacek <rnovacek@redhat.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PKAGENTEXAMPLE_H
#define PKAGENTEXAMPLE_H

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include <QCoreApplication>
#include "klistener.h"

class PkAgentExample : public QCoreApplication
{
    Q_OBJECT
public:
    PkAgentExample(int &argc, char **argv);
    ~PkAgentExample() override {};
private:
    KListener m_listener;
};


#endif
