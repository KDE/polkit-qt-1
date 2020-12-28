/*
    This file is part of the Polkit-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>
    SPDX-FileContributor: based on code by David Zeuthen <davidz@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT_LISTENER_P_H
#define POLKITQT_LISTENER_P_H

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include "listeneradapter_p.h"

#include <polkitagent/polkitagent.h>

G_BEGIN_DECLS

#define POLKIT_QT_TYPE_LISTENER          (polkit_qt_listener_get_type())
#define POLKIT_QT_LISTENER(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_QT_TYPE_LISTENER, PolkitQtListener))
#define POLKIT_QT_LISTENER_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_QT_TYPE_LISTENER, PolkitQtListenerClass))
#define POLKIT_QT_LISTENER_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_QT_TYPE_LISTENER, PolkitQtListenerClass))
#define POLKIT_QT_IS_LISTENER(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_QT_TYPE_LISTENER))
#define POLKIT_QT_IS_LISTENER_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_QT_TYPE_LISTENER))

typedef struct _PolkitQtListener PolkitQtListener;
typedef struct _PolkitQtListenerClass PolkitQtListenerClass;

GType                 polkit_qt_listener_get_type(void) G_GNUC_CONST;
PolkitAgentListener  *polkit_qt_listener_new(void);

G_END_DECLS

#endif /* POLKIT_QT_LISTENER_H */
