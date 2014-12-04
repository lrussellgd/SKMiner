////////////////////////////////////////////////
// File:	RequestHandler.cpp
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

#include "RequestHandler.h"
#include "MimeTypes.h"

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

#include <fstream>
#include <string>

#include "Request.h"
#include "Reply.h"
#include "Header.h"

#include <json_spirit.h>

namespace Http
{
	namespace Server
	{
		RequestHandler::RequestHandler(APIHandler* pAPIHandler) : m_pAPIHandler(pAPIHandler)
		{
		}

		///////////////////////////////////////////////////////////////////////////////
		//Destructor
		///////////////////////////////////////////////////////////////////////////////
		RequestHandler::~RequestHandler()
		{

		}

		void RequestHandler::HandleRequest(const Request* pRequest, Reply* pReply)
		{
			std::string szRequestPath;
			if (!UrlDecode(pRequest->GetUri(), szRequestPath))
			{
				pReply = Reply::StockReply(Reply::BAD_REQUEST);
				return;
			}

			if (szRequestPath.empty() || szRequestPath[0] != '/'
				|| szRequestPath.find("..") != std::string::npos)
			{
				pReply = Reply::StockReply(Reply::BAD_REQUEST);
				return;
			}

			std::size_t stLastSlashPos = szRequestPath.find_last_of("/");
			std::string szAPIQuery;
			std::string szAPICommand;
			std::map<std::string,std::string> mapParams;
			if (stLastSlashPos != std::string::npos)
			{
				szAPIQuery = szRequestPath.substr(stLastSlashPos + 1);
				std::size_t paramStart = szAPIQuery.find("?");
				if (paramStart != std::string::npos)
				{
					szAPICommand = szAPIQuery.substr(0, paramStart - 1);

					while (paramStart != std::string::npos)
					{
						std::size_t nextParam = szAPIQuery.find("&", paramStart);
						if (nextParam != std::string::npos)
						{
							std::string szParam = szAPIQuery.substr(paramStart, nextParam);
							size_t valStart = szParam.find("=");
							if (valStart == std::string::npos)
							{
								break;
							}

							std::string szKey = szParam.substr(0, valStart - 1);
							std::string szVal = szParam.substr(valStart + 1);

							mapParams[szKey] = szVal;

							paramStart = nextParam;
						}
						else
						{
							std::string szParam = szAPIQuery.substr(paramStart);
							size_t valStart = szParam.find("=");
							if (valStart == std::string::npos)
							{
								break;
							}

							std::string szKey = szParam.substr(0, valStart - 1);
							std::string szVal = szParam.substr(valStart + 1);

							mapParams[szKey] = szVal;

							paramStart = std::string::npos;
						}
					}
				}
				else
				{
					szAPICommand = szAPIQuery;
				}
			}

			//API Handling
			std::string szContent = m_pAPIHandler->HandleRequest(szAPICommand, mapParams);	

			pReply->SetStatus(Reply::OK);
			pReply->SetContent(szContent);

			pReply->ClearHeaders();

			Header* pHeader1 = new Header();
			pHeader1->SetName("Content-Length");
			pHeader1->SetValue(std::to_string(pReply->GetContent().size()));

			Header* pHeader2 = new Header();
			pHeader2->SetName("Content-Type");
			pHeader2->SetValue(MimeTypes::ExtensionToType(MimeTypes::ExtensionToType("json")));

			Header* pHeader3 = new Header();
			pHeader3->SetName("Access-Control-Allow-Origin");
			pHeader3->SetValue("*");
		
			pReply->AddHeader(pHeader1);
			pReply->AddHeader(pHeader2);
			pReply->AddHeader(pHeader3);
		}

		bool RequestHandler::UrlDecode(const std::string& szIn, std::string& szOut)
		{
			szOut.clear();
			szOut.reserve(szIn.size());
			for (std::size_t i = 0; i < szIn.size(); ++i)
			{
				if (szIn[i] == '%')
				{
					if (i + 3 <= szIn.size())
					{
						int value = 0;
						std::istringstream is(szIn.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							szOut += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (szIn[i] == '+')
				{
					szOut += ' ';
				}
				else
				{
					szOut += szIn[i];
				}
			}
			return true;
		}
	}
}