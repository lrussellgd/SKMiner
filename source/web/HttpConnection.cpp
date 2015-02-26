////////////////////////////////////////////////
// File:	HttpConnection.cpp
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

#include "HttpConnection.h"
#include <utility>
#include <vector>

#include "Reply.h"
#include "Request.h"
#include "RequestHandler.h"
#include "RequestParser.h"
#include "JsonRequestParser.h"
#include "ConnectionManager.h"
#include "APIHandler.h"

#include <iostream>
#include <fstream>

namespace Http
{
	namespace Server
	{
		HttpConnection::HttpConnection(LLP::Socket_t* pSocket, APIHandler* pHandler)
		{
			this->m_clLock = new boost::mutex();
			this->m_requestLock = new boost::mutex();
			this->m_dataLock = new boost::mutex();
			this->m_readLock = new boost::mutex();

			m_pSocket = pSocket;
			m_pAPIHandler = pHandler;

			m_pReply = NULL;
			m_pRequest = NULL;
			m_pRequestParser = NULL;
			m_pJsonParser = NULL;
			m_pRequestHandler = NULL;

			m_bStopped = false;

			m_pReadThread = NULL;

			m_n32LastTime = 0;
		}

		HttpConnection::~HttpConnection()
		{
			if (this->m_clLock)
			{
				this->m_clLock->unlock();
				delete(this->m_clLock);
				this->m_clLock = NULL;
			}

			if (this->m_requestLock)
			{
				this->m_requestLock->unlock();
				delete(this->m_requestLock);
				this->m_requestLock = NULL;
			}

			if (m_pReadThread)
			{
				m_pReadThread->detach();
				delete(m_pReadThread);
				m_pReadThread = NULL;
			}

			if (this->m_readLock)
			{
				this->m_readLock->unlock();
				delete(this->m_readLock);
				this->m_readLock = NULL;
			}

			ClearData();

			if (m_dataLock)
			{
				m_dataLock->unlock();
				delete(m_dataLock);
				m_dataLock = NULL;
			}
		}

		void HttpConnection::ClearData()
		{
			m_dataLock->lock();
			{
				if (m_pRequest)
				{
					delete(m_pRequest);
					m_pRequest = NULL;
				}

				if (m_pRequestParser)
				{
					delete(m_pRequestParser);
					m_pRequestParser = NULL;
				}

				if (m_pJsonParser)
				{
					delete(m_pJsonParser);
					m_pJsonParser = NULL;
				}

				if (m_pRequestHandler)
				{
					delete(m_pRequestHandler);
					m_pRequestHandler = NULL;
				}

				if (m_pReply)
				{
					delete(m_pReply);
					m_pReply = NULL;
				}

				m_tDataTimer.Reset();
				m_tDataTimer.Stop();
			}
			m_dataLock->unlock();
		}

		void HttpConnection::Start()
		{
			m_bStopped = false;
			StartRead();
		}

		void HttpConnection::Stop()
		{
			m_pSocket->get()->close();
			m_bStopped = true;
		}

		void HttpConnection::StartRead()
		{
			if (m_pReadThread)
			{
				m_pReadThread->detach();
				delete(m_pReadThread);
				m_pReadThread = NULL;
			}
			ClearData();


			m_pReadThread = new LLP::Thread_t(boost::thread(&HttpConnection::DoRead, this));
		}

		void HttpConnection::DoRead()
		{
			m_dataLock->lock();
			{
				boost::system::error_code ec;
				std::array<char, 8192> arrBuffer;
				std::size_t bytes_transferred = 0;
				bool bReadDone = false;
				bool bIsFirst = true;

				m_pReply = NULL;
				m_pRequest = new Request();
				m_pRequestParser = new RequestParser();
				m_pJsonParser = new JsonRequestParser();
			
				while (!bReadDone && (m_n32LastTime < TIME_OUT))
				{
					bytes_transferred = m_pSocket->get()->read_some(boost::asio::buffer(arrBuffer), ec);
					if (!ec)
					{
						if (bIsFirst)
						{
							m_tDataTimer.Start();
							bIsFirst = false;
							std::string dataBuff = std::string(arrBuffer.data());

							if (dataBuff.find("HTTP") != std::string::npos)
							{
								m_pRequest->SetRequestType(Http::Server::Request::RequestType::HTTP);
							}
							else if (dataBuff.find("{") != std::string::npos)
							{
								m_pRequest->SetRequestType(Http::Server::Request::RequestType::JSON);
							}
							else
							{
								m_pRequest->SetRequestType(Http::Server::Request::RequestType::STRING);
							}
						}

						m_n32LastTime = m_tDataTimer.ElapsedMilliseconds();

						RequestParser::ResultType result = RequestParser::ResultType::INDETERMINATE;
						switch (m_pRequest->GetRequestType())
						{
							case Http::Server::Request::RequestType::HTTP:
								{
									result = m_pRequestParser->Parse(m_pRequest, arrBuffer.data(), arrBuffer.data() + bytes_transferred);
								}
							break;
							case Http::Server::Request::RequestType::JSON:
								{
									result = m_pJsonParser->Parse(m_pRequest, arrBuffer.data(), arrBuffer.data() + bytes_transferred);
								}
								break;
							case Http::Server::Request::RequestType::STRING:
								{
									std::string szData = std::string(arrBuffer.data(), arrBuffer.data() + bytes_transferred);
									m_pRequest->SetMethod(szData);
									result = RequestParser::ResultType::GOOD;
								}
								break;
						}
					
						if (result == RequestParser::ResultType::GOOD)
						{
							m_pReply = new Reply();

							RequestHandler* pRequestHandler = new RequestHandler(m_pAPIHandler);
							pRequestHandler->HandleRequest(m_pRequest, m_pReply);
							delete(pRequestHandler);
							pRequestHandler = NULL;
							bReadDone = true;
						}
						else if (result == RequestParser::ResultType::BAD)
						{
							m_pReply = Reply::StockReply(Reply::BAD_REQUEST);
							bReadDone = true;
						}
					}
					else if (ec != boost::asio::error::operation_aborted)
					{
						bReadDone = true;
					}
				}

				m_tDataTimer.Reset();
				m_tDataTimer.Stop();

				if (m_pRequestParser)
				{
					delete(m_pRequestParser);
					m_pRequestParser = NULL;
				}

				if (m_pJsonParser)
				{
					delete(m_pJsonParser);
					m_pJsonParser = NULL;
				}

				if (m_pRequest)
				{
					delete(m_pRequest);
					m_pRequest = NULL;
				}

				if (!m_pReply)
				{
					Stop();
					return;
				}
			
				std::vector<boost::asio::const_buffer> vecBuffers = m_pReply->ToBuffers();
				boost::system::error_code writeError;

				boost::asio::write(*m_pSocket->get(), vecBuffers, writeError);

				if (m_pReply)
				{
					delete(m_pReply);
					m_pReply = NULL;
				}

				if (!writeError)
				{
					// Initiate graceful connection closure.	
					boost::system::error_code ignored_ec;
					m_pSocket->get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
					m_bStopped = true;
				}

				if (writeError != boost::asio::error::operation_aborted)
				{
					Stop();
				}
			}
			m_dataLock->unlock();
		}
	}
}