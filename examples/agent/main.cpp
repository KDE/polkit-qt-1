/*
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include "pkagentexample.h"

int main(int argc, char *argv[])
{
    PkAgentExample example(argc, argv);
    return example.exec();
}
