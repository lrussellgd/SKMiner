////////////////////////////////////////////////
// File:	Request.h
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

#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <string>
#include <vector>

namespace Http
{
	namespace Server
	{
		class Header;

		class Request
		{

		private:

			std::string m_szMethod;
			std::string m_szURI;
			int m_nHttpVersionMajor;
			int m_nHttpVersionMinor;
			std::vector<Header*> m_vecHeaders;

		public:

			//////////////////////////////////////////////////////////////////////////////
			//Constructor
			///////////////////////////////////////////////////////////////////////////////
			Request();

			///////////////////////////////////////////////////////////////////////////////
			//Copy Constructor
			///////////////////////////////////////////////////////////////////////////////
			Request(const Request& request);

			///////////////////////////////////////////////////////////////////////////////
			//Assignment Operator
			///////////////////////////////////////////////////////////////////////////////
			Request& operator=(const Request& request);

			///////////////////////////////////////////////////////////////////////////////
			//Destructor
			///////////////////////////////////////////////////////////////////////////////
			~Request();

			///////////////////////////////////////////////////////////////////////////////
			//Accessors
			///////////////////////////////////////////////////////////////////////////////
			const std::string&			GetMethod()				const { return this->m_szMethod;			}
			const std::string&			GetUri()				const { return this->m_szURI;				}
			const int					GetHttpVersionMajor()	const { return this->m_nHttpVersionMajor;	}
			const int					GetHttpVersionMinor()	const { return this->m_nHttpVersionMinor;	}
			const std::vector<Header*>& GetHeaders()			const { return this->m_vecHeaders;			}
			Header*						GetHeader(int nIndex)	const { if (this->m_vecHeaders.size() > nIndex) { return this->m_vecHeaders[nIndex]; } return NULL; }

			///////////////////////////////////////////////////////////////////////////////
			//Mutators
			///////////////////////////////////////////////////////////////////////////////
			void SetMethod(const std::string& szMethod)				{ this->m_szMethod = szMethod;				}
			void SetUri(const std::string& szUri)					{ this->m_szURI = szUri;					}
			void SetHttpVersionMajor(const int nVersion)			{ this->m_nHttpVersionMajor = nVersion;		}
			void SetHttpVersionMinor(const int nVersion)			{ this->m_nHttpVersionMinor = nVersion;		}
			void SetHeaders(const std::vector<Header*>& vecHeaders) { this->m_vecHeaders = vecHeaders;			}
			void AddHeader(Header* pHeader)							{ this->m_vecHeaders.push_back(pHeader);	}
		};
	}
}

#endif //_REQUEST_H_