#ifndef DETAILS_H
#define DETAILS_H

#include <QtCore/QObject>
#include <polkit/polkit.h>
#include "export.h"

/**
 * \namespace PolkitQt PolkitQt
 *
 * \brief Namespace wrapping Polkit-Qt classes
 *
 * This namespace wraps all Polkit-Qt classes.
 */
namespace PolkitQt
{

/**
 * \class Details details.h Details
 * \author Radek Novacek <rnovacek@redhat.com>
 *
 * \brief Class used for passing details around.
 */
class POLKIT_QT_EXPORT Details : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new Details object
     */
    Details(QObject *parent = NULL);

    ~Details();

    /**
     * Get the value for \p key
     *
     * \param key A key
     * \return Value of the key \p key, otherwise empty QString.
     */
    QString lookup(const QString &key) const;

    /**
     * Inserts key \p key with value \p value.
     *
     * \param key A key.
     * \param value A value.
     */
    void insert(const QString &key, const QString &value);

    /**
     * Gets a list of all keys.
     *
     * \return List of all keys.
     */
    QStringList getKeys() const;
private:
    PolkitDetails *m_polkitDetails;
};

}

#endif // DETAILS_H
