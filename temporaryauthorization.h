#ifndef TEMPORARYAUTHORIZATION_H
#define TEMPORARYAUTHORIZATION_H

#include <polkit/polkit.h>
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include "subject.h"

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
 * \class TemporaryAuthorization temporaryauthorization.h TemporaryAuthorization
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * \brief This class represents PolicyKit temporary authorization
 *
 * This class encapsulates the PolkitTemporaryAuthorization interface.
 */
class POLKIT_QT_EXPORT TemporaryAuthorization : public QObject
{
    Q_OBJECT
private:
    PolkitTemporaryAuthorization *m_temporaryAuthorization;
    QString m_id, m_actionId;
    Subject *m_subject;
    QDateTime m_timeObtained, m_timeExpires;
public:
    /**
     * \brief Gets the identifier for the authorization.
     *
     * This identifier can be user by the revokeTemporaryAuthorization function
     *
     * \return Unique identifier for the authorization
     */
    QString id();

    /**
     * \brief Gets the identifier of the action that authorization is for
     *
     * \return String that identifies the action
     */
    QString actionId();

    /**
     * \brief Gets the subject that authorization is for
     *
     * \return A Subject.
     */
    Subject *subject();

    /**
     * \brief Gets the time when authorization was obtained
     *
     * \return Time of obtaining the authorization
     */
    QDateTime timeObtained();

    /**
     * \brief Gets the time when authorizaton will expire
     *
     * \return Time of expiration
     */
    QDateTime timeExpires();

    TemporaryAuthorization(PolkitTemporaryAuthorization *tempAuth);

    /**
     * \brief Revoke temporary authorization
     *
     * \return \c true Authorization has been revoked
     *         \c false Revoking authorization failed
     */
    bool revoke();
};
}

#endif // TEMPORARYAUTHORIZATION_H
