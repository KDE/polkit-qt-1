/*
    This file is part of the PolKit1-qt project
    SPDX-FileCopyrightText: 2009 Jaroslav Reznik <jreznik@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef POLKITQT1_SUBJECT_H
#define POLKITQT1_SUBJECT_H

#include "polkitqt1-export.h"

#include <QObject>
#include <QSharedData>

typedef struct _PolkitSubject PolkitSubject;
typedef struct _PolkitUnixProcess PolkitUnixProcess;
typedef struct _PolkitSystemBusName PolkitSystemBusName;

/**
 * \namespace PolkitQt1 PolkitQt
 *
 * \brief Namespace wrapping PolicyKit-Qt classes
 *
 * This namespace wraps all PolicyKit-Qt classes.
 */
namespace PolkitQt1
{

class UnixUserIdentity;

/**
 * \class Subject polkitqt1-subject.h Subject
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
class POLKITQT1_EXPORT Subject
{
public:
    Subject();
    Subject(const Subject &other);
    ~Subject();

    Subject &operator=(const Subject &other);

    bool isValid() const;

    /**
     * Serialization of object to the string
     *
     * \return Serialized Subject object
     */
    QString toString() const;

    /**
     * Creates the Subject object from string reprezentation
     *
     * \param string string reprezentation of the object
     *
     * \return Pointer to new Subject instance
     */
    static Subject fromString(const QString &string);

    /**
     * Gets PolkitSubject object.
     *
     * \warning It shouldn't be used directly unless you are completely aware of what are you doing
     *
     * \return Pointer to PolkitSubject instance
     */
    PolkitSubject *subject() const;

protected:
    Subject(PolkitSubject *subject);

    void setSubject(PolkitSubject *subject);

private:
    class Data;
    QExplicitlySharedDataPointer< Data > d;
};

/**
 * \class UnixProcessSubject polkitqt1-subject.h Subject
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief A class for representing a UNIX process.
 *
 * To uniquely identify processes, both the process
 * id and the start time of the process (a monotonic
 * increasing value representing the time since the
 * kernel was started) is used.
 *
 * \sa Subject
 */
class POLKITQT1_EXPORT UnixProcessSubject : public Subject
{
public:
    /**
    * Subject constructor, takes one parameter - PID. The start time
    * of process will be looked automatically.
    *
    * \param pid An Unix process PID.
    */
    explicit UnixProcessSubject(qint64 pid);

    /**
    * Subject constructor, takes two parameters - PID and start time.
    *
    * \param pid An Unix process PID.
    * \param startTime An Unix process start time.
    */
    UnixProcessSubject(qint64 pid, quint64 startTime);

    /**
     * Subject constructor, it creates UnixProcess object from PolkitUnixProcess object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param process PolkitUnixProcess object
     */
    explicit UnixProcessSubject(PolkitUnixProcess *process);

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
    * Returns Unix process UID.
    *
    * \return A UID of associated Unix process.
    */
    qint64 uid() const;

    /**
    * Sets Unix process PID.
    *
    * \param pid An Unix process PID.
    */
    void setPid(qint64 pid);
};

/**
 * \class SystemBusNameSubject polkitqt1-subject.h Subject
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief A class for representing a process owning a unique name on the system bus.
 *
 * \sa Subject
 */
class POLKITQT1_EXPORT SystemBusNameSubject : public Subject
{
public:
    /**
    * Subject constructor, takes one parameter - system bus name.
    *
    * \param name A unique system bus name.
    */
    explicit SystemBusNameSubject(const QString &name);

    /**
     * Subject constructor, it creates SystemBusName object from PolkitSystemBusName object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkSystemBusName PolkitSystemBusName object
     */
    explicit SystemBusNameSubject(PolkitSystemBusName *pkSystemBusName);

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

    /**
     * Returns the UnixUserIdentity for this subject.
     *
     * \note This can be an invalid UnixUserIdentity so be sure to check before using it
     *
     * \since 0.113
     **/
    UnixUserIdentity user();
};

/**
 * \class UnixSessionSubject polkitqt1-subject.h Subject
 * \author Jaroslav Reznik <jreznik@redhat.com>
 *
 * \brief A class for representing unix session.
 *
 * The session id is an opaque string obtained from
 * ConsoleKit.
 *
 * \sa Subject
 */
class POLKITQT1_EXPORT UnixSessionSubject : public Subject
{
public:
    /**
    * Subject constructor, takes one parameter - session id.
    *
    * \param sessionId The session id.
    */
    explicit UnixSessionSubject(const QString &sessionId);

    /**
    * Subject constructor, takes one parameter - pid of process.
    *
    * Synchronous!
    *
    * \param pid The session's process pid.
    */
    explicit UnixSessionSubject(qint64 pid);

    /**
     * Subject constructor, it creates UnixSession object from PolkitUnixSession object
     *
     * \warning Use this only if you are completely aware of what are you doing!
     *
     * \param pkUnixSession PolkitUnixSession object
     */
    explicit UnixSessionSubject(PolkitSystemBusName *pkUnixSession);

    /**
    * Returns session id.
    *
    * \return A session id.
    */
    QString sessionId() const;

    /**
    * Sets session id.
    *
    * \param sessionId A session id.
    */
    void setSessionId(const QString &sessionId);
};

}

#endif
