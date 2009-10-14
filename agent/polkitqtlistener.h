/*
 * This file is part of the Polkit-qt project
 * Copyright (C) 2009 Jaroslav Reznik <jreznik@redhat.com>
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
 *
 * polkit-qt-listener based on code by David Zeuthen <davidz@redhat.com>
 */

#ifndef POLKIT_QT_LISTENER_H
#define POLKIT_QT_LISTENER_H

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include "listeneradapter.h"

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

GType                 polkit_qt_listener_get_type   (void) G_GNUC_CONST;
PolkitAgentListener  *polkit_qt_listener_new        (void);

G_END_DECLS

#endif /* POLKIT_QT_LISTENER_H */
