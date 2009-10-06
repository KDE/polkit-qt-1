#include "details.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>

using namespace PolkitQt;

Details::Details(QObject *parent) : QObject(parent)
{
    g_type_init();
    m_polkitDetails = polkit_details_new();
}

Details::~Details()
{
    g_object_unref(m_polkitDetails);
}

QString Details::lookup(const QString &key) const
{
    const gchar *result = polkit_details_lookup(m_polkitDetails, key.toUtf8().data());
    if (result != NULL)
        return QString::fromUtf8(result);
    else
        return QString();
}

void Details::insert(const QString &key, const QString &value)
{
    polkit_details_insert(m_polkitDetails, key.toUtf8().data(), value.toUtf8().data());
}

QStringList Details::getKeys() const
{
    gchar **result = polkit_details_get_keys(m_polkitDetails);
    QStringList list;
    int len = g_strv_length(result);
    for (int i = 0; i < len; i++)
    {
        list.append(QString::fromUtf8(result[i]));
    }
    g_strfreev(result);
    return list;
}
