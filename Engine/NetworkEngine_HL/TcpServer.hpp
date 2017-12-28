#pragma once

#include <TcpListener.hpp>

#include "ServerConnectionHandler.hpp"

namespace inl::net::servers
{
	using namespace inl::net::sockets;

	class TcpServer
	{
	public:
		TcpServer(uint32_t max_connections, uint16_t port);

		void Start();

	private:
		void BeginAccept();
		void EndAccept();

	private:
		TcpListener *listener;
		uint32_t m_maxConnections;
		uint16_t m_port;
		bool m_run;

		std::thread m_acceptingThread;

		ServerConnectionHandler m_connectionPool;
	};
}