////////////////////////////////////////////////
// File:	Request.cpp
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

#include "Request.h"

#include "Header.h"

namespace Http
{
	namespace Server
	{
		//////////////////////////////////////////////////////////////////////////////
		//Constructor
		///////////////////////////////////////////////////////////////////////////////
		Request::Request()
		{
			this->m_nHttpVersionMajor = 0;
			this->m_nHttpVersionMinor = 0;
			this->m_szMethod = "";
			this->m_szURI = "";
		}

		///////////////////////////////////////////////////////////////////////////////
		//Copy Constructor
		///////////////////////////////////////////////////////////////////////////////
		Request::Request(const Request& request)
		{
			this->m_szMethod = request.GetMethod();
			this->m_szURI = request.GetUri();
			this->m_nHttpVersionMajor = request.GetHttpVersionMajor();
			this->m_nHttpVersionMinor = request.GetHttpVersionMinor();
			this->m_vecHeaders = request.GetHeaders();
		}

		///////////////////////////////////////////////////////////////////////////////
		//Assignment Operator
		///////////////////////////////////////////////////////////////////////////////
		Request& Request::operator = (const Request& request)
		{
			this->m_szMethod = request.GetMethod();
			this->m_szURI = request.GetUri();
			this->m_nHttpVersionMajor = request.GetHttpVersionMajor();
			this->m_nHttpVersionMinor = request.GetHttpVersionMinor();
			this->m_vecHeaders = request.GetHeaders();

			return *this;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Destructor
		///////////////////////////////////////////////////////////////////////////////
		Request::~Request()
		{
			for (size_t index = 0; index < this->m_vecHeaders.size(); ++index)
			{
				Header* pHeader = this->m_vecHeaders[index];
				if (pHeader)
				{
					delete(pHeader);
					pHeader = NULL;
				}
			}
			this->m_vecHeaders.clear();
		}
	}
}