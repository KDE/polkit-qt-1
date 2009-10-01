#include "session.h"

#include <QtCore/QDebug>
#include <QtGui/QInputDialog>
#include "identity.h"

using namespace PolkitQtAgent;

Session::Session(PolkitQt::Identity *identity, const QString &cookie)
{
    m_polkitAgentSession = polkit_agent_session_new(identity->identity(), cookie.toUtf8().data());
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "completed", G_CALLBACK(_completed), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "request", G_CALLBACK(_request), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "show-error", G_CALLBACK(_showError), this);
    g_signal_connect(G_OBJECT(m_polkitAgentSession), "show-info", G_CALLBACK(_showInfo), this);
}

void Session::initiate()
{
    polkit_agent_session_initiate(m_polkitAgentSession);
}

void Session::response(const QString &response)
{
    polkit_agent_session_response(m_polkitAgentSession, response.toUtf8().data());
}

void Session::cancel()
{
    polkit_agent_session_cancel(m_polkitAgentSession);
}

void Session::_completed(PolkitAgentSession *s, gboolean gained_authorizaition, gpointer user_data)
{
    qDebug() << "COMPLETED";
    emit ((Session *)user_data)->completed(gained_authorizaition);
}

void Session::_request(PolkitAgentSession *s, gchar *request, gboolean echo_on, gpointer user_data)
{
    qDebug() << "REQUEST";
    emit ((Session *)user_data)->request(QString::fromUtf8(request), echo_on);
}

void Session::_showError(PolkitAgentSession *s, gchar *text, gpointer user_data)
{
    qDebug() << "showError";
    emit ((Session *)user_data)->showError(QString::fromUtf8(text));
}

void Session::_showInfo(PolkitAgentSession *s, gchar *text, gpointer user_data)
{
    qDebug() << "showInfo";
    emit ((Session *)user_data)->showInfo(QString::fromUtf8(text));
}
