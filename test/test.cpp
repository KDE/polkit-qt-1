

#include "test.h"
#include "core/polkitqt1-authority.h"
#include "agent/polkitqt1-agent-session.h"
#include "core/polkitqt1-details.h"
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
using namespace PolkitQt1;
using namespace PolkitQt1::Agent;

void wait()
{
    for (int i = 0; i < 100; i++) {
        usleep(100);
        QCoreApplication::processEvents();
    }
}

void TestAuth::test_Auth_checkAuthorization()
{
    // This needs the file org.qt.policykit.examples.policy from examples to be installed
    UnixProcessSubject process(QCoreApplication::applicationPid());
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
    QSignalSpy spy(authority, SIGNAL(checkAuthorizationFinished(PolkitQt1::Authority::Result)));
    // Call asynchronous checkAuthorization
    authority->checkAuthorization("org.qt.policykit.examples.kick", process, Authority::None);
    // Give the polkit time to obtain the result and emit the signal with it
    wait();
    // Test if the signal was emitted
    QCOMPARE(spy.count(), 1);
    // Test the result
    result = qVariantValue<PolkitQt1::Authority::Result> (spy.takeFirst()[0]);
    QCOMPARE(result, Authority::No);
    QVERIFY(!authority->hasError());
    spy.clear();

    // Let's test the cancellability
    authority->checkAuthorization("org.qt.policykit.examples.kick", process, Authority::None);
    authority->checkAuthorizationCancel();
    // Wait and check if the signal arrieved
    wait();
    QCOMPARE(spy.count(), 0);

    // Check if it can cancel user authentication dialog
    authority->checkAuthorization("org.qt.policykit.examples.bleed", process, Authority::AllowUserInteraction);
    // Show it for second
    sleep(1);
    // And now kill it
    authority->checkAuthorizationCancel();
    QVERIFY(!authority->hasError());
    // But how to test if it was successful?
    qWarning() << "You should see an authentication dialog for a short period.";
}

void TestAuth::test_Auth_enumerateActions()
{
    // This needs the file org.qt.policykit.examples.policy from examples to be installed
    ActionDescription::List list = Authority::instance()->enumerateActionsSync();
    QVERIFY(!Authority::instance()->hasError());
    // Check whether enumerateAction returns at least example actions
    int count = 0;
    Q_FOREACH(const ActionDescription &ad, list) {
        if ((ad.actionId() == "org.qt.policykit.examples.kick") ||
                (ad.actionId() == "org.qt.policykit.examples.cry") ||
                (ad.actionId() == "org.qt.policykit.examples.bleed"))
            count++;
    }
    QCOMPARE(count, 3);


    // Test asynchronous version as well
    list.clear();
    count = 0;
    QSignalSpy spy(Authority::instance(), SIGNAL(enumerateActionsFinished(PolkitQt1::ActionDescription::List)));
    Authority::instance()->enumerateActions();
    wait();
    QCOMPARE(spy.count(), 1);
    list = qVariantValue<PolkitQt1::ActionDescription::List> (spy.takeFirst()[0]);
    QVERIFY(!Authority::instance()->hasError());
    Q_FOREACH(const ActionDescription &ad, list) {
        if ((ad.actionId() == "org.qt.policykit.examples.kick") ||
                (ad.actionId() == "org.qt.policykit.examples.cry") ||
                (ad.actionId() == "org.qt.policykit.examples.bleed"))
            count++;
    }
    QCOMPARE(count, 3);

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
    struct passwd *userinfo = getpwuid(getuid());
    QString userName = userinfo->pw_name;
    unsigned int userId = userinfo->pw_uid;
    unsigned int groupId = userinfo->pw_gid;

    // Try to create UnixUser from username
    UnixUserIdentity user(userName);
    QVERIFY(user.identity());

    // Create generic Identity from UnixUser via string representation
    Identity id = Identity::fromString(user.toString());
    // Compare obtained uid with real uid
    QCOMPARE(id.toUnixUserIdentity().uid(), userId);

    // Create generic Identity from UnixGroup via string representation
    UnixGroupIdentity group(groupId);
    QVERIFY(group.identity());
    id = Identity::fromString(group.toString());
    QCOMPARE(id.toUnixGroupIdentity().gid(), groupId);

    // Test setting gid to another value
    group.setGid(9999U);
    id = Identity::fromString(group.toString());
    QCOMPARE(id.toUnixGroupIdentity().gid(), 9999U);
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
    UnixProcessSubject *process = new UnixProcessSubject(pid);
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
    QList<QString> list = details.keys();
    QVERIFY(list.contains("1"));
    QVERIFY(list.contains("2"));
    QVERIFY(list.contains("3"));
    QVERIFY(list.contains("4"));
}

QTEST_MAIN(TestAuth)
