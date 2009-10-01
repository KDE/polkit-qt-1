#ifndef TEST_H
#define TEST_H

#include <QtCore/QObject>
#include <QtTest/QtTest>

class TestAuth : public QObject
{
    Q_OBJECT
private slots:
    void test_Auth_checkAuthorization();
    void test_Auth_enumerateActions();
    void test_Identity();
    void test_Authority();
};

#endif // TEST_H
