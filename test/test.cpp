

#include "test.h"
#include "authority.h"
#include "session.h"
#include "details.h"
#include <stdlib.h>
#include <unistd.h>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
using namespace PolkitQt;
using namespace PolkitQtAgent;

void wait()
{
    for (int i = 0; i < 100; i++)
    {
        usleep(100);
        QCoreApplication::processEvents();
    }
}

void TestAuth::test_Auth_checkAuthorization()
{
    // This needs the file org.qt.policykit.examples.policy from examples to be installed
    UnixProcess *process = new UnixProcess(QCoreApplication::applicationPid());
    Authority::Result result;
    // Check if this method returns good authorization results
    Authority *authority = Authority::instance();
    result = authority->checkAuthorizationSync("org.qt.policykit.examples.kick", process, Authority::None);
    QCOMPARE(result, Authority::No);
    QVERIFY(!authority->hasError());
    result = authority->checkAuthorizationSync("org.qt.policykit.examples.cry", process, Authority::None);
    QCOMPARE(result, Authority::Yes);
    QVERIFY(!authority->hasError());
    result = authority->checkAuthorizationSync("org.qt.policykit.examples.bleed", process, Authority::None);
    QCOMPARE(result, Authority::Challenge);
    QVERIFY(!authority->hasError());

    // Now we try async methods
    QSignalSpy spy(authority, SIGNAL(checkAuthorizationFinished(PolkitQt::Authority::Result)));
    // Call asynchronous checkAuthorization
    authority->checkAuthorization("org.qt.policykit.examples.kick", process, Authority::None);
    // Give the polkit time to obtain the result and emit the signal with it
    wait();
    // Test if the signal was emitted
    QCOMPARE(spy.count(), 1);
    // Test the result
    result = qVariantValue<PolkitQt::Authority::Result> (spy.takeFirst()[0]);
    QCOMPARE(result, Authority::No);
    QVERIFY(!authority->hasError());
    spy.clear();

    // Let's test the cancellability
    authority->checkAuthorization("org.qt.policykit.examples.kick", process, Authority::None);
    authority->checkAuthorizationCancel();
    // Wait and check if the signal arrieved
    wait();
    QCOMPARE(spy.count(), 0);

    // Check if it can cancel user authentification dialog
    authority->checkAuthorization("org.qt.policykit.examples.bleed", process, Authority::AllowUserInteraction);
    // Show it for second
    sleep(1);
    // And now kill it
    authority->checkAuthorizationCancel();
    QVERIFY(!authority->hasError());
    // But how to test if it was successfull?
    qWarning() << "You should see authentification dialog for short period.";
}

void TestAuth::test_Auth_enumerateActions()
{
    // This needs the file org.qt.policykit.examples.policy from examples to be installed
    QStringList list = Authority::instance()->enumerateActionsSync();
    QVERIFY(!Authority::instance()->hasError());
    // Check whether enumerateAction returns at least example actions
    QVERIFY(list.contains("org.qt.policykit.examples.kick"));
    QVERIFY(list.contains("org.qt.policykit.examples.cry"));
    QVERIFY(list.contains("org.qt.policykit.examples.bleed"));

    // Test asynchronous version as well
    QSignalSpy spy(Authority::instance(), SIGNAL(enumerateActionsFinished(QStringList)));
    Authority::instance()->enumerateActions();
    wait();
    QCOMPARE(spy.count(), 1);
    list = qVariantValue<QStringList> (spy.takeFirst()[0]);
    QVERIFY(!Authority::instance()->hasError());
    QVERIFY(list.contains("org.qt.policykit.examples.kick"));
    QVERIFY(list.contains("org.qt.policykit.examples.cry"));
    QVERIFY(list.contains("org.qt.policykit.examples.bleed"));

    // Test cancelling the enumeration
    spy.clear();
    Authority::instance()->enumerateActions();
    Authority::instance()->enumerateActionsCancel();
    wait();
    QCOMPARE(spy.count(), 0);
    QVERIFY(!Authority::instance()->hasError());
}

void TestAuth::test_Identity()
{
    // Get real name and id of current user and group
    QString userName = QString::fromAscii(getlogin());
    unsigned int userId = getuid();
    unsigned int groupId = getpgid(QCoreApplication::applicationPid());

    // Try to create UnixUser from username
    UnixUserIdentity user(userName);
    QVERIFY(user.identity());

    // Create generic Identity from UnixUser via string representation
    Identity *id = Identity::fromString(user.toString());
    // Compare obtained uid with real uid
    QCOMPARE(((UnixUserIdentity *)id)->uid(), userId);
    delete id;

    // Create generic Identity from UnixGroup via string representation
    UnixGroupIdentity group(groupId);
    QVERIFY(group.identity());
    id = Identity::fromString(group.toString());
    QCOMPARE(((UnixGroupIdentity *) id)->gid(), groupId);
    delete id;

    // Test setting gid to another value
    group.setGid(9999U);
    id = Identity::fromString(group.toString());
    QCOMPARE(((UnixGroupIdentity *) id)->gid(), 9999U);
    delete id;
}

void TestAuth::test_Authority()
{
    Authority *authority = Authority::instance();
    QVERIFY(authority);
    QVERIFY(!authority->hasError());

    // Verify emiting of the signals
    QSignalSpy spy(authority, SIGNAL(consoleKitDBChanged()));
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.ConsoleKit",
                                                      "/org/freedesktop/ConsoleKit/Manager",
                                                      "org.freedesktop.ConsoleKit.Manager",
                                                      "OpenSession");
    QDBusMessage reply = QDBusConnection::systemBus().call(msg);
    QString cookie;
    cookie = qVariantValue<QString> (reply.arguments()[0]);


    msg = QDBusMessage::createMethodCall("org.freedesktop.ConsoleKit",
                                                      "/org/freedesktop/ConsoleKit/Manager",
                                                      "org.freedesktop.ConsoleKit.Manager",
                                                      "CloseSession");
    msg.setArguments(QList<QVariant> () << cookie);
    QDBusConnection::systemBus().call(msg);
    // FIXME: Emitting consoleKitDBChanged is not working now
    qWarning() << "Emitting consoleKitDBChanged is not working now, test will be skipped";
    //QVERIFY(spy.count() > 0);
    QVERIFY(!authority->hasError());

    // configChanged signal from authority requires changing some policy files
    // and it would require user interaction (typing the password)
    // so this is not covered by this test
}

void TestAuth::test_Subject()
{
    // Get pid of this appication
    qint64 pid = QCoreApplication::applicationPid();
    // Create unix process for it
    UnixProcess *process = new UnixProcess(pid);
    // Test if pid doesn't differ
    QCOMPARE(process->pid(), pid);

    // Serialize and deserialize subject
    //Subject *subject = Subject::fromString(process->toString());
    // and try it
    //QCOMPARE(((UnixProcess *) subject)->pid(), pid);
    delete process;
}

void TestAuth::test_Session()
{
    /*
    UnixUser user(getuid());
    Session *session = new Session(&user, "/org/freedesktop/ConsoleKit/Session2");
    QSignalSpy spy_completed(session, SIGNAL(completed(bool)));
    QSignalSpy spy_request(session, SIGNAL(request(QString,bool)));
    QSignalSpy spy_error(session, SIGNAL(showError(QString)));
    QSignalSpy spy_info(session, SIGNAL(showInfo(QString)));
    session->initiate();
    session->response("aaa");
    // Canceling should emit the "completed" signal
    session->cancel();
    QCOMPARE(spy_completed.count(), 1);

    //UnixProcess *process = new UnixProcess(QCoreApplication::applicationPid());
    //Authority::instance()->checkAuthorization("org.qt.policykit.examples.kick", process, Authority::None);

    qDebug() << "COMPLETED:" << spy_completed.count();
    qDebug() << "REQUEST:" << spy_request.count();
    qDebug() << "ERROR:" << spy_error.count();
    qDebug() << "INFO:" << spy_info.count();
    */
}

void TestAuth::test_Details()
{
    Details details;
    details.insert("1", "aaa");
    details.insert("2", "bbb");
    details.insert("3", "ccc");
    details.insert("4", "ddd");
    QCOMPARE(details.lookup("1"), QString("aaa"));
    QCOMPARE(details.lookup("2"), QString("bbb"));
    QCOMPARE(details.lookup("3"), QString("ccc"));
    QCOMPARE(details.lookup("4"), QString("ddd"));
    QList<QString> list = details.getKeys();
    QVERIFY(list.contains("1"));
    QVERIFY(list.contains("2"));
    QVERIFY(list.contains("3"));
    QVERIFY(list.contains("4"));
}

QTEST_MAIN(TestAuth)
