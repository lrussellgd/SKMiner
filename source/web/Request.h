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

#include "../base/Entity.h"

#include <string>
#include <vector>

namespace Http
{
	namespace Server
	{
		class Header;

		class Request : public Entity
		{

		public:

			enum RequestType
			{
				HTTP,
				JSON,
				STRING
			};

		private:

			int m_n32ID;

			bool m_bIsTypeSet;
			RequestType m_rtType;

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
			const bool					GetIsTypeSet()			const { return this->m_bIsTypeSet;			}	
			const Request::RequestType	GetRequestType()		const { return this->m_rtType;				}
			const std::string&			GetMethod()				const { return this->m_szMethod;			}
			const std::string&			GetUri()				const { return this->m_szURI;				}
			const int					GetID()					const { return this->m_n32ID;				}
			const int					GetHttpVersionMajor()	const { return this->m_nHttpVersionMajor;	}
			const int					GetHttpVersionMinor()	const { return this->m_nHttpVersionMinor;	}
			const std::vector<Header*>& GetHeaders()			const { return this->m_vecHeaders;			}
			Header*						GetHeader(int nIndex)	const { if (this->m_vecHeaders.size() > nIndex) { return this->m_vecHeaders[nIndex]; } return NULL; }

			///////////////////////////////////////////////////////////////////////////////
			//Mutators
			///////////////////////////////////////////////////////////////////////////////
			void SetIsTypeSet(const bool bIsTypeSet)				{ this->m_bIsTypeSet = bIsTypeSet;			}
			void SetRequestType(const Request::RequestType rtType)	{ this->m_rtType = rtType;					}
			void SetMethod(const std::string& szMethod)				{ this->m_szMethod = szMethod;				}
			void SetUri(const std::string& szUri)					{ this->m_szURI = szUri;					}
			void SetID(const int n32ID)								{ this->m_n32ID = n32ID;					}	
			void SetHttpVersionMajor(const int nVersion)			{ this->m_nHttpVersionMajor = nVersion;		}
			void SetHttpVersionMinor(const int nVersion)			{ this->m_nHttpVersionMinor = nVersion;		}
			void SetHeaders(const std::vector<Header*>& vecHeaders) { this->m_vecHeaders = vecHeaders;			}
			void AddHeader(Header* pHeader)							{ this->m_vecHeaders.push_back(pHeader);	}
		};
	}
}

#endif //_REQUEST_H_