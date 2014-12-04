////////////////////////////////////////////////
// File:	RequestParser.cpp
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

#include "RequestParser.h"

#include "Request.h"
#include "Header.h"

namespace Http
{
	namespace Server
	{
		//////////////////////////////////////////////////////////////////////////////
		//Constructor
		///////////////////////////////////////////////////////////////////////////////
		RequestParser::RequestParser()
		{
			this->m_eState = METHOD_START;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Copy Constructor
		///////////////////////////////////////////////////////////////////////////////
		RequestParser::RequestParser(const RequestParser& requestParser)
		{
			this->m_eState = requestParser.GetState();
		}

		///////////////////////////////////////////////////////////////////////////////
		//Assignment Operator
		///////////////////////////////////////////////////////////////////////////////
		RequestParser& RequestParser::operator = (const RequestParser& requestParser)
		{
			this->m_eState = requestParser.GetState();

			return *this;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Destructor
		///////////////////////////////////////////////////////////////////////////////
		RequestParser::~RequestParser()
		{

		}

		bool RequestParser::IsChar(int nCH)
		{
			return (nCH >= 0 && nCH <= 127);
		}

		bool RequestParser::IsCtl(int nCH)
		{
			return ((nCH >= 0 && nCH <= 31) || (nCH == 127));
		}

		bool RequestParser::IsTSpecial(int nCH)
		{
			switch (nCH)
			{
				case '(': 
				case ')': 
				case '<': 
				case '>': 
				case '@':
				case ',': 
				case ';': 
				case ':': 
				case '\\': 
				case '"':
				case '/': 
				case '[': 
				case ']': 
				case '?': 
				case '=':
				case '{': 
				case '}': 
				case ' ': 
				case '\t':
					return true;
				default:
					return false;
			}
		}

		bool RequestParser::IsDigit(int nCH)
		{
			return (nCH >= '0' && nCH <= '9');
		}

		void RequestParser::Reset()
		{
			this->m_eState = METHOD_START;
		}

		RequestParser::ResultType RequestParser::Parse(Request* pRequest, char* itBegin, char* itEnd)
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

		RequestParser::ResultType RequestParser::Consume(Request* pRequest, char chInput)
		{
			switch (m_eState)
			{
				case METHOD_START:
					{
						if (!IsChar(chInput) || IsCtl(chInput) || IsTSpecial(chInput))
						{
							return RequestParser::ResultType::BAD;
						}
						else
						{
							m_eState = METHOD;
							pRequest->SetMethod(pRequest->GetMethod() + chInput);

							return RequestParser::ResultType::INDETERMINATE;
						}
					}
					break;
				case METHOD:
					{

						if (chInput == ' ')
						{
							m_eState = URI;
							return RequestParser::ResultType::INDETERMINATE;
						}
						else if (!IsChar(chInput) || IsCtl(chInput) || IsTSpecial(chInput))
						{
							return RequestParser::ResultType::BAD;
						}
						else
						{
							pRequest->SetMethod(pRequest->GetMethod() + chInput);
							return RequestParser::ResultType::INDETERMINATE;
						}
					}
					break;
				case URI:
					{
						if (chInput == ' ')
						{
							m_eState = HTTP_VERSION_H;
							return RequestParser::ResultType::INDETERMINATE;
						}
						else if (IsCtl(chInput))
						{
							return RequestParser::ResultType::BAD;
						}
						else
						{
							pRequest->SetUri(pRequest->GetUri() + chInput);
							return RequestParser::ResultType::INDETERMINATE;
						}
					}
					break;
				case HTTP_VERSION_H:
					{
							if (chInput == 'H')
							{
								m_eState = HTTP_VERSION_T_1;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_T_1:
					{
							if (chInput == 'T')
							{
								m_eState = HTTP_VERSION_T_2;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_T_2:
					{
							if (chInput == 'T')
							{
								m_eState = HTTP_VERSION_P;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_P:
					{
							if (chInput == 'P')
							{
								m_eState = HTTP_VERSION_SLASH;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_SLASH:
					{
							if (chInput == '/')
							{
								pRequest->SetHttpVersionMajor(0);
								pRequest->SetHttpVersionMinor(0);
								m_eState = HTTP_VERSION_MAJOR_START;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_MAJOR_START:
					{
							if (IsDigit(chInput))
							{
								pRequest->SetHttpVersionMajor(pRequest->GetHttpVersionMajor() * 10 + chInput - '0');
								m_eState = HTTP_VERSION_MAJOR;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_MAJOR:
					{
							if (chInput == '.')
							{
								m_eState = HTTP_VERSION_MINOR_START;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (IsDigit(chInput))
							{
								pRequest->SetHttpVersionMajor(pRequest->GetHttpVersionMajor() * 10 + chInput - '0');
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_MINOR_START:
					{
							if (IsDigit(chInput))
							{
								pRequest->SetHttpVersionMinor(pRequest->GetHttpVersionMinor() * 10 + chInput - '0');
								m_eState = HTTP_VERSION_MINOR;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HTTP_VERSION_MINOR:
					{
							if (chInput == '\r')
							{
								m_eState = EXPECTING_NEWLINE_1;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (IsDigit(chInput))
							{
								pRequest->SetHttpVersionMinor(pRequest->GetHttpVersionMinor() * 10 + chInput - '0');
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case EXPECTING_NEWLINE_1:
					{
							if (chInput == '\n')
							{
								m_eState = HEADER_LINE_START;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HEADER_LINE_START:
					{
							if (chInput == '\r')
							{
								m_eState = EXPECTING_NEWLINE_3;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (!pRequest->GetHeaders().empty() && (chInput == ' ' || chInput == '\t'))
							{
								m_eState = HEADER_LWS;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (!IsChar(chInput) || IsCtl(chInput) || IsTSpecial(chInput))
							{
								return RequestParser::ResultType::BAD;
							}
							else
							{
								Header* pHeader = new Header();
								pHeader->SetName(pHeader->GetName() + chInput);
								pRequest->AddHeader(pHeader);

								m_eState = HEADER_NAME;
								return RequestParser::ResultType::INDETERMINATE;
							}
					}
					break;
				case HEADER_LWS:
					{
							if (chInput == '\r')
							{
								m_eState = EXPECTING_NEWLINE_2;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (chInput == ' ' || chInput == '\t')
							{
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (IsCtl(chInput))
							{
								return RequestParser::ResultType::BAD;
							}
							else
							{
								m_eState = HEADER_VALUE;
								pRequest->GetHeaders().back()->SetValue(pRequest->GetHeaders().back()->GetValue() + chInput);
								return RequestParser::ResultType::INDETERMINATE;
							}
					}
					break;
				case HEADER_NAME:
					{
							if (chInput == ':')
							{
								m_eState = SPACE_BEFORE_HEADER_VALUE;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (!IsChar(chInput) || IsCtl(chInput) || IsTSpecial(chInput))
							{
								return RequestParser::ResultType::BAD;
							}
							else
							{
								pRequest->GetHeaders().back()->SetName(pRequest->GetHeaders().back()->GetName() + chInput);
								return RequestParser::ResultType::INDETERMINATE;
							}
					}
					break;
				case SPACE_BEFORE_HEADER_VALUE:
					{
							if (chInput == ' ')
							{
								m_eState = HEADER_VALUE;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case HEADER_VALUE:
					{
							if (chInput == '\r')
							{
								m_eState = EXPECTING_NEWLINE_2;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else if (IsCtl(chInput))
							{
								return RequestParser::ResultType::BAD;
							}
							else
							{
								pRequest->GetHeaders().back()->SetValue(pRequest->GetHeaders().back()->GetValue() + chInput);
								return RequestParser::ResultType::INDETERMINATE;
							}
					}
					break;
				case EXPECTING_NEWLINE_2:
					{
							if (chInput == '\n')
							{
								m_eState = HEADER_LINE_START;
								return RequestParser::ResultType::INDETERMINATE;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				case EXPECTING_NEWLINE_3:
					{
							if (chInput == '\n')
							{
								return RequestParser::ResultType::GOOD;
							}
							else
							{
								return RequestParser::ResultType::BAD;
							}
					}
					break;
				default:
					{
						   return RequestParser::ResultType::BAD;
					}
					break;
			}
		}
	}
}