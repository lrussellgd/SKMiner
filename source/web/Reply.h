////////////////////////////////////////////////
// File:	Reply.h
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

#ifndef _REPLY_H_
#define _REPLY_H_

#include "../base/Entity.h"

#include <vector>
#include <string>
#include <boost/asio.hpp>

namespace Http
{
	namespace Server
	{
		class Header;

		class Reply : public Entity
		{

		public:

			enum StatusType
			{
				OK = 200,
				CREATED = 201,
				ACCEPTED = 202,
				NO_CONTENT = 204,
				MULTIPLE_CHOICES = 300,
				MOVED_PERMANENTLY = 301,
				MOVED_TEMPORARILY = 302,
				NOT_MOTIFIED = 304,
				BAD_REQUEST = 400,
				UNAUTHORIZED = 401,
				FORBIDDEN = 403,
				NOT_FOUND = 404,
				INTERNAL_SERVER_ERROR = 500,
				NOT_IMPLEMENTED = 501,
				BAD_GATEWAY = 502,
				SERVICE_UNAVAILABLE = 503
			};

		private:

			int m_n32ID;
			StatusType m_eStatusType;
			std::vector<Header*> m_vecHeaders;

			std::string m_szContent;

		public:

			//////////////////////////////////////////////////////////////////////////////
			//Constructor
			///////////////////////////////////////////////////////////////////////////////
			Reply();

			///////////////////////////////////////////////////////////////////////////////
			//Copy Constructor
			///////////////////////////////////////////////////////////////////////////////
			Reply(const Reply& reply);

			///////////////////////////////////////////////////////////////////////////////
			//Assignment Operator
			///////////////////////////////////////////////////////////////////////////////
			Reply& operator=(const Reply& reply);

			///////////////////////////////////////////////////////////////////////////////
			//Destructor
			///////////////////////////////////////////////////////////////////////////////
			~Reply();

			std::vector<boost::asio::const_buffer> ToBuffers();
			static Reply* StockReply(StatusType eStatusType);

			///////////////////////////////////////////////////////////////////////////////
			//Accessors
			///////////////////////////////////////////////////////////////////////////////
			const int					GetID()					const	{ return this->m_n32ID;			}
			const std::vector<Header*>& GetHeaders()			const	{ return this->m_vecHeaders;	}
			Header*						GetHeader(size_t index) const	{ if (this->m_vecHeaders.size() > index) { return this->m_vecHeaders[index]; } return NULL; }
			const StatusType&			GetStatus()				const	{ return this->m_eStatusType;	}
			const std::string&			GetContent()			const	{ return this->m_szContent;		}

			///////////////////////////////////////////////////////////////////////////////
			//Mutators
			///////////////////////////////////////////////////////////////////////////////
			void SetID(const int n32ID)									{ this->m_n32ID = n32ID;					}
			void SetHeaders(const std::vector<Header*>& vecHeaders)		{ this->m_vecHeaders = vecHeaders;			}
			void AddHeader(Header* pHeader)								{ this->m_vecHeaders.push_back(pHeader);	}
			void SetStatus(const StatusType& eStatusType)				{ this->m_eStatusType = eStatusType;		}		
			void SetContent(const std::string& szContent)				{ this->m_szContent = szContent;			}
			void AddContent(const std::string szContent)				{ this->m_szContent += szContent;			}
			void AddContent(const char* pContent, size_t stCount = 0)	{ if (!stCount){ this->m_szContent.append(pContent); } else{ this->m_szContent.append(pContent, stCount); } }

			void ClearHeaders();
		};
	}
}


#endif //_REPLY_H_