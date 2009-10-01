#include "temporaryauthorization.h"
#include "authority.h"

using namespace PolkitQt;

TemporaryAuthorization::TemporaryAuthorization(PolkitTemporaryAuthorization *tempAuth) : QObject(NULL)
{
    g_type_init();
    m_id = QString::fromUtf8(polkit_temporary_authorization_get_id(tempAuth));
    m_actionId = QString::fromUtf8(polkit_temporary_authorization_get_action_id(tempAuth));
    m_subject = Subject::fromString(polkit_subject_to_string(polkit_temporary_authorization_get_subject(tempAuth)));
    m_timeObtained = QDateTime::fromTime_t(polkit_temporary_authorization_get_time_obtained(tempAuth));
    m_timeExpires = QDateTime::fromTime_t(polkit_temporary_authorization_get_time_expires(tempAuth));
}

QString TemporaryAuthorization::id()
{
    return m_id;
}

QString TemporaryAuthorization::actionId()
{
    return m_actionId;
}

Subject *TemporaryAuthorization::subject()
{
    //qFatal(polkit_subject_to_string(polkit_temporary_authorization_get_subject(m_temporaryAuthorization)));
    return m_subject;//Subject::fromString(polkit_subject_to_string(m_subject));
}

QDateTime TemporaryAuthorization::timeObtained()
{
    return m_timeObtained;
}

QDateTime TemporaryAuthorization::timeExpires()
{
    return m_timeExpires;
}

bool TemporaryAuthorization::revoke()
{
    Authority::instance()->revokeTemporaryAuthorization(id());
}
