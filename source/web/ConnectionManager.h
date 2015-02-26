////////////////////////////////////////////////
// File:	ConnectionManager.h
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

#include <set>
#include "../base/Entity.h"
#include "HttpConnection.h"

namespace Http
{
	namespace Server
	{
		class ConnectionManager : public Entity
		{

		private:

			std::set<HttpConnection*> m_setConnections;

		public:

			ConnectionManager(const ConnectionManager&) = delete;
			ConnectionManager& operator=(const ConnectionManager&) = delete;

			ConnectionManager();

			/// Add the specified connection to the manager and start it.
			void Start(HttpConnection* c);

			/// Stop the specified connection.
			void Stop(HttpConnection* c);

			/// Stop all connections.
			void StopAll();
		};
	}
}