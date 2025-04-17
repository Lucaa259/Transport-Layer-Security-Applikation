#pragma once

#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

class TLSApplikation;
class ClientSocket
{
public:
	ClientSocket(TLSApplikation& app, const std::string& IP, const std::string& Port);
	~ClientSocket() = default;

	const SOCKET& GetSocket();

private:
	void Connect();

private:
	TLSApplikation* m_pApp;
	boost::asio::io_context m_IoCtx;
	tcp::socket m_Socket;
	std::string m_strIP;
	unsigned short m_nPort;
};

