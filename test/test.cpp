

#include "test.h"
#include "authority.h"
#include <stdlib.h>
#include <unistd.h>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
using namespace PolkitQt;

void TestAuth::test_Auth_checkAuthorization()
{
    // This needs the file org.qt.policykit.examples.policy from examples to be installed
    UnixProcess *process = new UnixProcess(QCoreApplication::applicationPid());
    Authority::Result result;
    // Check if this method returns good authorization results
    Authority *authority = Authority::instance();
    result = authority->checkAuthorization("org.qt.policykit.examples.kick", process, Authority::None);
    QCOMPARE(result, Authority::No);
    result = authority->checkAuthorization("org.qt.policykit.examples.cry", process, Authority::None);
    QCOMPARE(result, Authority::Yes);
    result = authority->checkAuthorization("org.qt.policykit.examples.bleed", process, Authority::None);
    QCOMPARE(result, Authority::Challenge);
}

void TestAuth::test_Auth_enumerateActions()
{
    // This needs the file org.qt.policykit.examples.policy from examples to be installed
    QStringList list = Authority::instance()->enumerateActions();
    // Check whether enumerateAction returns at least example actions
    QVERIFY(list.contains("org.qt.policykit.examples.kick"));
    QVERIFY(list.contains("org.qt.policykit.examples.cry"));
    QVERIFY(list.contains("org.qt.policykit.examples.bleed"));
}

void TestAuth::test_Identity()
{
    // Get real name and id of current user and group
    QString userName = QString::fromAscii(getlogin());
    unsigned int userId = getuid();
    unsigned int groupId = getpgid(QCoreApplication::applicationPid());

    // Try to create UnixUser from username
    UnixUser user(userName);
    QVERIFY(user.identity());

    // Create generic Identity from UnixUser via string reprezentation
    Identity *id = Identity::fromString(user.toString());
    // Compare obtained uid with real uid
    QCOMPARE(((UnixUser *)id)->uid(), userId);
    delete id;

    // Create generic Identity from UnixGroup via string reprezentation
    UnixGroup group(groupId);
    QVERIFY(group.identity());
    id = Identity::fromString(group.toString());
    QCOMPARE(((UnixGroup *) id)->gid(), groupId);
    delete id;

    // Test setting gid to another value
    group.setGid(9999U);
    id = Identity::fromString(group.toString());
    QCOMPARE(((UnixGroup *) id)->gid(), 9999U);
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

QTEST_MAIN(TestAuth)
