/*
 * This file is part of the PolKit1-qt project
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
 */

#ifndef POLICYKIT_QT_SUBJECT_H
#define POLICYKIT_QT_SUBJECT_H

#include "export.h"

#include <QtCore/QObject>
#include <polkit/polkit.h>

/**
 * \namespace PolkitQt PolkitQt
 *
 * \brief Namespace wrapping PolicyKit-Qt classes
 *
 * This namespace wraps all PolicyKit-Qt classes.
 */
namespace PolkitQt
{

/**
 * \class Subject subject.h Subject
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief This class represents PolicyKit subjects 
 *
 * This class encapsulates the PolkitSubject interface.
 * 
 * \see UnixProcess
 * \see SystemBusName
 * \see UnixSession
 */    
class POLKIT_QT_EXPORT Subject : public QObject
{
    Q_OBJECT
public:
    ~Subject();

    /**
     * Serialization of object to the string
     *
     * \return Serialized Subject object
     */
    QString toString() const;

    /**
     * Creates the Subject object from string reprezentation
     *
     * \param String reprezentation of the object
     *
     * \return Pointer to new Subject instance
     */
    static Subject * fromString(const QString & string);

    /**
     * Gets PolkitSubject object.
     *
     * \warning It shouldn't be used directly unless you are completely aware of what are you doing
     *
     * \return Pointer to PolkitSubject instance
     */
    PolkitSubject * subject();
protected:
    PolkitSubject * m_subject;
    explicit Subject(QObject *parent = 0);
    Subject(PolkitSubject *subject, QObject *parent);
};

/**
 * \class UnixProcess subject.h UnixProcess
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief A class for representing a UNIX process. 
 *
 * To uniquely identify processes, both the process
 * id and the start time of the process (a monotonic 
 * increasing value representing the time since the 
 * kernel was started) is used.
 * 
 */    
class POLKIT_QT_EXPORT UnixProcess: public Subject
{
    Q_OBJECT;
public:
    /**
    * Subject constructor, takes one parameter - PID. The start time
    * of process will be looked automaticaly.
    *
    * \param pid An Unix process PID.
    */    
    UnixProcess(qint64 pid, QObject *parent = 0);

    /**
    * Subject constructor, takes two parameters - PID and start time.
    *
    * \param pid An Unix process PID.
    * \param startTime An Unix process start time.
    */    
    UnixProcess(qint64 pid, quint64 startTime, QObject *parent = 0);

    /**
     * Subject constructor, it creates UnixProcess object from PolkitUnixProcess object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixProcess PolkitUnixProcess object
     */
    UnixProcess(PolkitUnixProcess *process, QObject *parent = 0);
    
    /**
    * Returns Unix process PID.
    *
    * \return A PID of associated Unix process.
    */    
    qint64 pid() const;

    /**
    * Returns Unix process start time.
    *
    * \return A start time of associated Unix process.
    */    
    qint64 startTime() const;
    
    /**
    * Sets Unix process PID.
    *
    * \param pid An Unix process PID.
    */
    void setPid(qint64 pid);
};

/**
 * \class SystemBusName subject.h SystemBusName
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief A class for representing a process owning a unique name on the system bus. 
 */    
class POLKIT_QT_EXPORT SystemBusName: public Subject
{
    Q_OBJECT;
public:
    /**
    * Subject constructor, takes one parameter - system bus name.
    *
    * \param name A unique system bus name.
    */    
    SystemBusName(const QString &name, QObject *parent = 0);

    /**
     * Subject constructor, it creates SystemBusName object from PolkitSystemBusName object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkSystemBusName PolkitSystemBusName object
     */
    SystemBusName(PolkitSystemBusName *pkSystemBusName, QObject *parent = 0);

    /**
    * Returns system bus name.
    *
    * \return A unique system bus name.
    */    
    QString name() const;
    
    /**
    * Sets system bus name.
    *
    * \param name System bus name.
    */
    void setName(const QString &name);
    
    // TODO: unixProcess
};

/**
 * \class UnixSession subject.h UnixSession
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief A class for representing unix session.
 *
 * The session id is an opaque string obtained from 
 * ConsoleKit.
 */    
class POLKIT_QT_EXPORT UnixSession: public Subject
{
    Q_OBJECT;
public:
    /**
    * Subject constructor, takes one parameter - session id.
    *
    * \param sessionId The session id.
    */    
    UnixSession(const QString &sessionId, QObject *parent = 0);
    
    /**
    * Subject constructor, takes one parameter - pid of process.
    * 
    * Synchronous!
    *
    * \param pid The session's process pid.
    */  
    UnixSession(qint64 pid, QObject *parent = 0);

    /**
     * Subject constructor, it creates UnixSession object from PolkitUnixSession object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixSession PolkitUnixSession object
     */
    UnixSession(PolkitSystemBusName *pkUnixSession, QObject *parent = 0);

    /**
    * Returns session id.
    *
    * \return A session id.
    */    
    QString sessionId() const;
    
    /**
    * Sets session id.
    *
    * \param name A session id.
    */
    void setSessionId(const QString &sessionId);
    
    // TODO: constructor from PID?
};

}

#endif
