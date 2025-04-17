#include "ServerSocket.h"
#include "TLSApplikation.h"

ServerSocket::ServerSocket(TLSApplikation& app, const std::string& Port)
	: m_pApp(&app),
	m_Acceptor(m_IoCtx, tcp::endpoint(tcp::v4(), static_cast<unsigned short>(std::stoi(Port)))),
	m_ClientSocket(m_IoCtx)
{
	StartAcceptConnections();
}

ServerSocket::~ServerSocket()
{
	std::string test = "test test";
	std::cout << test << std::endl;
	// to do look here
}


void ServerSocket::StartAcceptConnections()
{
	try 
	{
		m_Acceptor.accept(m_ClientSocket);
	}
	catch (const std::exception& e)
	{
		std::string strError = ("Error: " + std::string(e.what()) + "\n");

		if (!m_pApp)
			m_pApp->SetStatus(strError);
	}
}

const SOCKET& ServerSocket::GetSocket()
{
	return static_cast<SOCKET>(m_ClientSocket.native_handle());
}
