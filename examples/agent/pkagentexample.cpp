#include <polkitagent/polkitagent.h>
#include "polkitqtlistener.h"
#include "pkagentexample.h"
#include <glib-object.h>
#include <QtCore/QDebug>
#include "klistener.h"

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

PkAgentExample::PkAgentExample(int &argc, char **argv)
        : QCoreApplication(argc, argv)
{
    g_type_init();

    m_listener.registerListener("/org/kde/PolicyKit1/AuthenticationAgent");
}
