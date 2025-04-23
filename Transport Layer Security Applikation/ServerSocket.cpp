#include "ServerSocket.h"
#include "TLSApplikation.h"

ServerSocket::ServerSocket(TLSApplikation& app, const std::string& Port)
	: m_pApp(&app),
	m_Acceptor(m_IoCtx, tcp::endpoint(tcp::v4(), static_cast<unsigned short>(std::stoi(Port)))),
	m_ClientSocket(m_IoCtx)
{
}

ServerSocket::~ServerSocket()
{
}


SOCKET ServerSocket::StartAcceptConnections()
{
	try
	{
		m_Acceptor.accept(m_ClientSocket);  // blockierend
		return static_cast<SOCKET>(m_ClientSocket.native_handle());
	}
	catch (const std::exception& error)
	{
		std::string strError = "Exception: " + std::string(error.what()) + "\n";
		if (m_pApp)
			m_pApp->SetStatus(strError);
	}

	return INVALID_SOCKET;
}
