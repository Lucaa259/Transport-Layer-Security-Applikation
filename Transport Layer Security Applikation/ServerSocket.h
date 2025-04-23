#pragma once

#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::cout;

class TLSApplikation;
class ServerSocket
{
public:
	ServerSocket(TLSApplikation& app, const std::string& Port);
	~ServerSocket();

	SOCKET StartAcceptConnections();

private:
	TLSApplikation* m_pApp;
	boost::asio::io_context m_IoCtx;
	tcp::acceptor m_Acceptor;
	tcp::socket m_ClientSocket;
};

