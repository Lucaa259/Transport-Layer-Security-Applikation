#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace boost::asio;
using ip::tcp;
using boost::asio::ip::address;
using std::string;

class Server {
	
};

class Client 
{
	public:
		Client(const std::string& IP, const std::string& Port);
		~Client() = default;

	private:
		boost::asio::io_context m_IoCtx;
		std::unique_ptr<tcp::socket> m_Socket;
};

class Socket
{
};
