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
#include "ConnectionManager.h"

#include <iostream>
#include <fstream>

namespace Http
{
	namespace Server
	{

		HttpConnection::HttpConnection(LLP::Socket_t* pSocket,
			ConnectionManager* pManager, RequestHandler* pHandler)
		{
			m_pSocket = pSocket;
			m_pConnectionManager = pManager;
			m_pRequestHandler = pHandler;

			m_pRequestParser = new RequestParser();

			m_pRequest = new Request();
			m_pReply = NULL;
		}

		HttpConnection::~HttpConnection()
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

			if (m_pReply)
			{
				delete(m_pReply);
				m_pReply = NULL;
			}
		}

		void HttpConnection::Start()
		{
			DoRead();
		}

		void HttpConnection::Stop()
		{
			m_pSocket->get()->close();
		}

		void HttpConnection::DoRead()
		{
			auto self(shared_from_this());
			m_pSocket->get()->async_read_some(boost::asio::buffer(m_arrBuffer),
				[this, self](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					RequestParser::ResultType result = RequestParser::ResultType::INDETERMINATE;

					result = m_pRequestParser->Parse(m_pRequest, m_arrBuffer.data(), m_arrBuffer.data() + bytes_transferred);

					if (result == RequestParser::ResultType::GOOD)
					{
						m_pReply = new Reply();
						m_pRequestHandler->HandleRequest(m_pRequest, m_pReply);
						delete(m_pRequest);
						m_pRequest = new Request();
						DoWrite();
					}
					else if (result == RequestParser::ResultType::BAD)
					{
						m_pReply = Reply::StockReply(Reply::BAD_REQUEST);
						delete(m_pRequest);
						m_pRequest = new Request();
						DoWrite();
					}
					else
					{
						DoRead();
					}
				}
				else if (ec != boost::asio::error::operation_aborted)
				{
					m_pConnectionManager->Stop(shared_from_this());
				}
			});
		}

		void HttpConnection::DoWrite()
		{
			auto self(shared_from_this());
			boost::asio::async_write(*m_pSocket->get(), m_pReply->ToBuffers(),
				[this, self](boost::system::error_code ec, std::size_t)
			{
				delete(m_pReply);
				m_pReply = NULL;

				if (!ec)
				{
					// Initiate graceful connection closure.
					boost::system::error_code ignored_ec;
					m_pSocket->get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both,	ignored_ec);
				}

				if (ec != boost::asio::error::operation_aborted)
				{
					m_pConnectionManager->Stop(shared_from_this());
				}
			});
		}
	}
}