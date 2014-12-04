////////////////////////////////////////////////
// File:	ConnectionManager.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
//			Based off of the boost http server example
//			http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "ConnectionManager.h"

namespace Http
{
	namespace Server
	{
		ConnectionManager::ConnectionManager()
		{
		}

		void ConnectionManager::Start(connection_ptr c)
		{
			m_setConnections.insert(c);
			c->Start();
		}

		void ConnectionManager::Stop(connection_ptr c)
		{
			m_setConnections.erase(c);
			c->Stop();
		}

		void ConnectionManager::StopAll()
		{
			for (auto c : m_setConnections)
			{
				c->Stop();
			}
			m_setConnections.clear();
		}

	}
}