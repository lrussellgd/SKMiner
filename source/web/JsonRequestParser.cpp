////////////////////////////////////////////////
// File:	JsonRequestParser.cpp
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

#include "JsonRequestParser.h"

#include "Request.h"
#include "Header.h"

namespace Http
{
	namespace Server
	{
		//////////////////////////////////////////////////////////////////////////////
		//Constructor
		///////////////////////////////////////////////////////////////////////////////
		JsonRequestParser::JsonRequestParser()
		{
			this->m_enmEntityType = ENTITY_TYPE::HTTP_JSON_REQUEST_PARSER;
			m_nStartChars = 0;
			m_nEndChars = 0;
			this->m_eState = JSON_START;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Copy Constructor
		///////////////////////////////////////////////////////////////////////////////
		JsonRequestParser::JsonRequestParser(const JsonRequestParser& requestParser) : Entity(requestParser)
		{
			this->m_eState = requestParser.GetState();
		}

		///////////////////////////////////////////////////////////////////////////////
		//Assignment Operator
		///////////////////////////////////////////////////////////////////////////////
		JsonRequestParser& JsonRequestParser::operator = (const JsonRequestParser& requestParser)
		{
			this->m_enmEntityType = requestParser.GetEntityType();
			this->m_eState = requestParser.GetState();

			return *this;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Destructor
		///////////////////////////////////////////////////////////////////////////////
		JsonRequestParser::~JsonRequestParser()
		{

		}

		void JsonRequestParser::Reset()
		{
			m_nStartChars = 0;
			m_nEndChars = 0;
			this->m_eState = JSON_START;
		}

		RequestParser::ResultType JsonRequestParser::Parse(Request* pRequest, char* itBegin, char* itEnd)
		{
			while (itBegin != itEnd)
			{
				RequestParser::ResultType tbResult = Consume(pRequest, *itBegin++);
				if (tbResult == RequestParser::ResultType::GOOD || tbResult == RequestParser::ResultType::BAD)
				{
					return tbResult;
					//return boost::make_tuple(tbResult, begin);
				}
			}

			return RequestParser::ResultType::INDETERMINATE;
		}

		RequestParser::ResultType JsonRequestParser::Consume(Request* pRequest, char chInput)
		{
			switch (m_eState)
			{
				case JSON_START:
				{
					if (chInput != '{')
					{
						return RequestParser::ResultType::BAD;
					}
					else
					{
						m_nStartChars++;
						m_eState = JSON_INTERMEDIATE;
						pRequest->SetMethod(pRequest->GetMethod() + chInput);

						return RequestParser::ResultType::INDETERMINATE;
					}
				}
				break;
				case JSON_INTERMEDIATE:
				{
					if (chInput == '{')
					{
						m_nStartChars++;
					}
					else if (chInput == '}')
					{
						m_nEndChars++;
					}

					pRequest->SetMethod(pRequest->GetMethod() + chInput);

					if (m_nStartChars == m_nEndChars)
					{
						return RequestParser::ResultType::GOOD;
					}

					return RequestParser::ResultType::INDETERMINATE;
				}
				break;
				default:
				{
					return RequestParser::ResultType::BAD;
				}
			}

		}
	}
}