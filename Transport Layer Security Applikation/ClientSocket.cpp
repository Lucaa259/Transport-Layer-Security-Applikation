#include "ClientSocket.h"
#include "TLSApplikation.h"

ClientSocket::ClientSocket(TLSApplikation& app, const std::string& IP, const std::string& Port)
	:	  m_pApp(&app),
	m_Socket(m_IoCtx),
	m_strIP(IP),
	m_nPort(static_cast<unsigned short>(std::stoi(Port)))
{	
	Connect();
}

const SOCKET& ClientSocket::GetSocket()
{
	return static_cast<SOCKET>(m_Socket.native_handle());
}

void ClientSocket::Connect()
{
	try 
	{
		m_Socket.connect(tcp::endpoint(boost::asio::ip::make_address(m_strIP), m_nPort));
	}
	catch (std::exception& error)
	{
		std::string strError = "Error: " + std::string(error.what()) + "\n";
		m_pApp->SetStatus(strError);
	}	
}
