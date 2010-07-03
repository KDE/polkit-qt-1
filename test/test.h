#ifndef TEST_H
#define TEST_H

#include <QtCore/QObject>
#include <QtTest/QtTest>

class TestAuth : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test_Auth_checkAuthorization();
    void test_Auth_enumerateActions();
    void test_Identity();
    void test_Authority();
    void test_Subject();
    void test_Session();
    void test_Details();
};

#endif // TEST_H
