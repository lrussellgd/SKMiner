////////////////////////////////////////////////
// File:	RequestParser.h
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

#ifndef _REQUESTPARSER_H_
#define _REQUESTPARSER_H_

#include "../base/Entity.h"

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace Http
{
	namespace Server
	{
		class Request;

		class RequestParser : public Entity
		{

		public:

			enum ResultType { GOOD, BAD, INDETERMINATE };

		private:

			enum State
			{
				METHOD_START,
				METHOD,
				URI,
				HTTP_VERSION_H,
				HTTP_VERSION_T_1,
				HTTP_VERSION_T_2,
				HTTP_VERSION_P,
				HTTP_VERSION_SLASH,
				HTTP_VERSION_MAJOR_START,
				HTTP_VERSION_MAJOR,
				HTTP_VERSION_MINOR_START,
				HTTP_VERSION_MINOR,
				EXPECTING_NEWLINE_1,
				HEADER_LINE_START,
				HEADER_LWS,
				HEADER_NAME,
				SPACE_BEFORE_HEADER_VALUE,
				HEADER_VALUE,
				EXPECTING_NEWLINE_2,
				EXPECTING_NEWLINE_3
			};

			State m_eState;

			ResultType Consume(Request* pRequest, char chInput);
			static bool IsChar(int nCH);
			static bool IsCtl(int nCH);
			static bool IsTSpecial(int nCH);
			static bool IsDigit(int nCH);

		public:

			

			//////////////////////////////////////////////////////////////////////////////
			//Constructor
			///////////////////////////////////////////////////////////////////////////////
			RequestParser();

			///////////////////////////////////////////////////////////////////////////////
			//Copy Constructor
			///////////////////////////////////////////////////////////////////////////////
			RequestParser(const RequestParser& requestParser);

			///////////////////////////////////////////////////////////////////////////////
			//Assignment Operator
			///////////////////////////////////////////////////////////////////////////////
			RequestParser& operator=(const RequestParser& requestParser);

			///////////////////////////////////////////////////////////////////////////////
			//Destructor
			///////////////////////////////////////////////////////////////////////////////
			~RequestParser();


			void Reset();
		
			RequestParser::ResultType Parse(Request* pRequest, char* itBegin, char* itEnd);

			///////////////////////////////////////////////////////////////////////////////
			//Accessors
			///////////////////////////////////////////////////////////////////////////////
			const State& GetState() const { return this->m_eState; }
		};
	}
}


#endif //_REQUESTPARSER_H_