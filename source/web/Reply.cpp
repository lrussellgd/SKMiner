////////////////////////////////////////////////
// File:	Reply.cpp
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
//////////////////////////////////////////////

#include "Reply.h"

#include "Header.h"

namespace Http
{
	namespace Server
	{
		namespace StatusStrings {

			const std::string ok =
				"HTTP/1.0 200 OK\r\n";
			const std::string created =
				"HTTP/1.0 201 Created\r\n";
			const std::string accepted =
				"HTTP/1.0 202 Accepted\r\n";
			const std::string no_content =
				"HTTP/1.0 204 No Content\r\n";
			const std::string multiple_choices =
				"HTTP/1.0 300 Multiple Choices\r\n";
			const std::string moved_permanently =
				"HTTP/1.0 301 Moved Permanently\r\n";
			const std::string moved_temporarily =
				"HTTP/1.0 302 Moved Temporarily\r\n";
			const std::string not_modified =
				"HTTP/1.0 304 Not Modified\r\n";
			const std::string bad_request =
				"HTTP/1.0 400 Bad Request\r\n";
			const std::string unauthorized =
				"HTTP/1.0 401 Unauthorized\r\n";
			const std::string forbidden =
				"HTTP/1.0 403 Forbidden\r\n";
			const std::string not_found =
				"HTTP/1.0 404 Not Found\r\n";
			const std::string internal_server_error =
				"HTTP/1.0 500 Internal Server Error\r\n";
			const std::string not_implemented =
				"HTTP/1.0 501 Not Implemented\r\n";
			const std::string bad_gateway =
				"HTTP/1.0 502 Bad Gateway\r\n";
			const std::string service_unavailable =
				"HTTP/1.0 503 Service Unavailable\r\n";

			boost::asio::const_buffer ToBuffer(Reply::StatusType status)
			{
				switch (status)
				{
				case Reply::OK:
					return boost::asio::buffer(ok);
				case Reply::CREATED:
					return boost::asio::buffer(created);
				case Reply::ACCEPTED:
					return boost::asio::buffer(accepted);
				case Reply::NO_CONTENT:
					return boost::asio::buffer(no_content);
				case Reply::MULTIPLE_CHOICES:
					return boost::asio::buffer(multiple_choices);
				case Reply::MOVED_PERMANENTLY:
					return boost::asio::buffer(moved_permanently);
				case Reply::MOVED_TEMPORARILY:
					return boost::asio::buffer(moved_temporarily);
				case Reply::NOT_MOTIFIED:
					return boost::asio::buffer(not_modified);
				case Reply::BAD_REQUEST:
					return boost::asio::buffer(bad_request);
				case Reply::UNAUTHORIZED:
					return boost::asio::buffer(unauthorized);
				case Reply::FORBIDDEN:
					return boost::asio::buffer(forbidden);
				case Reply::NOT_FOUND:
					return boost::asio::buffer(not_found);
				case Reply::INTERNAL_SERVER_ERROR:
					return boost::asio::buffer(internal_server_error);
				case Reply::NOT_IMPLEMENTED:
					return boost::asio::buffer(not_implemented);
				case Reply::BAD_GATEWAY:
					return boost::asio::buffer(bad_gateway);
				case Reply::SERVICE_UNAVAILABLE:
					return boost::asio::buffer(service_unavailable);
				default:
					return boost::asio::buffer(internal_server_error);
				}
			}

		} // namespace status_strings

		namespace MiscStrings {

			const char NameValueSeparator[] = { ':', ' ' };
			const char CRLF[] = { '\r', '\n' };

		} // namespace misc_strings

		//////////////////////////////////////////////////////////////////////////////
		//Constructor
		///////////////////////////////////////////////////////////////////////////////
		Reply::Reply()
		{
			this->m_enmEntityType = ENTITY_TYPE::HTTP_REPLY;
			this->m_eStatusType = INTERNAL_SERVER_ERROR;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Copy Constructor
		///////////////////////////////////////////////////////////////////////////////
		Reply::Reply(const Reply& reply) : Entity(reply)
		{
			this->m_szContent = reply.GetContent();
			this->m_eStatusType = reply.GetStatus();
			this->m_vecHeaders = reply.GetHeaders();
		}

		///////////////////////////////////////////////////////////////////////////////
		//Assignment Operator
		///////////////////////////////////////////////////////////////////////////////
		Reply& Reply::operator = (const Reply& reply)
		{
			this->m_enmEntityType = reply.GetEntityType();
			this->m_szContent = reply.GetContent();
			this->m_eStatusType = reply.GetStatus();
			this->m_vecHeaders = reply.GetHeaders();

			return *this;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Destructor
		///////////////////////////////////////////////////////////////////////////////
		Reply::~Reply()
		{
			for (size_t index = 0; index < m_vecHeaders.size(); ++index)
			{
				Header* pHeader = m_vecHeaders[index];
				if (pHeader)
				{
					delete(pHeader);
					pHeader = NULL;
				}
			}
			m_vecHeaders.clear();
		}

		std::vector<boost::asio::const_buffer> Reply::ToBuffers()
		{
			std::vector<boost::asio::const_buffer> buffers;
			buffers.push_back(StatusStrings::ToBuffer(m_eStatusType));

			for (std::size_t index = 0; index < m_vecHeaders.size(); ++index)
			{
				Header* pHeader = m_vecHeaders[index];
				
				buffers.push_back(boost::asio::buffer(pHeader->GetName()));
				buffers.push_back(boost::asio::buffer(MiscStrings::NameValueSeparator));
				buffers.push_back(boost::asio::buffer(pHeader->GetValue()));
				buffers.push_back(boost::asio::buffer(MiscStrings::CRLF));
			}

			buffers.push_back(boost::asio::buffer(MiscStrings::CRLF));
			buffers.push_back(boost::asio::buffer(m_szContent));

			return buffers;
		}

		namespace StockReplies {

			const char ok[] = "";
			const char created[] =
				"<html>"
				"<head><title>Created</title></head>"
				"<body><h1>201 Created</h1></body>"
				"</html>";
			const char accepted[] =
				"<html>"
				"<head><title>Accepted</title></head>"
				"<body><h1>202 Accepted</h1></body>"
				"</html>";
			const char no_content[] =
				"<html>"
				"<head><title>No Content</title></head>"
				"<body><h1>204 Content</h1></body>"
				"</html>";
			const char multiple_choices[] =
				"<html>"
				"<head><title>Multiple Choices</title></head>"
				"<body><h1>300 Multiple Choices</h1></body>"
				"</html>";
			const char moved_permanently[] =
				"<html>"
				"<head><title>Moved Permanently</title></head>"
				"<body><h1>301 Moved Permanently</h1></body>"
				"</html>";
			const char moved_temporarily[] =
				"<html>"
				"<head><title>Moved Temporarily</title></head>"
				"<body><h1>302 Moved Temporarily</h1></body>"
				"</html>";
			const char not_modified[] =
				"<html>"
				"<head><title>Not Modified</title></head>"
				"<body><h1>304 Not Modified</h1></body>"
				"</html>";
			const char bad_request[] =
				"<html>"
				"<head><title>Bad Request</title></head>"
				"<body><h1>400 Bad Request</h1></body>"
				"</html>";
			const char unauthorized[] =
				"<html>"
				"<head><title>Unauthorized</title></head>"
				"<body><h1>401 Unauthorized</h1></body>"
				"</html>";
			const char forbidden[] =
				"<html>"
				"<head><title>Forbidden</title></head>"
				"<body><h1>403 Forbidden</h1></body>"
				"</html>";
			const char not_found[] =
				"<html>"
				"<head><title>Not Found</title></head>"
				"<body><h1>404 Not Found</h1></body>"
				"</html>";
			const char internal_server_error[] =
				"<html>"
				"<head><title>Internal Server Error</title></head>"
				"<body><h1>500 Internal Server Error</h1></body>"
				"</html>";
			const char not_implemented[] =
				"<html>"
				"<head><title>Not Implemented</title></head>"
				"<body><h1>501 Not Implemented</h1></body>"
				"</html>";
			const char bad_gateway[] =
				"<html>"
				"<head><title>Bad Gateway</title></head>"
				"<body><h1>502 Bad Gateway</h1></body>"
				"</html>";
			const char service_unavailable[] =
				"<html>"
				"<head><title>Service Unavailable</title></head>"
				"<body><h1>503 Service Unavailable</h1></body>"
				"</html>";

			std::string ToString(Reply::StatusType status)
			{
				switch (status)
				{
				case Reply::OK:
					return ok;
				case Reply::CREATED:
					return created;
				case Reply::ACCEPTED:
					return accepted;
				case Reply::NO_CONTENT:
					return no_content;
				case Reply::MULTIPLE_CHOICES:
					return multiple_choices;
				case Reply::MOVED_PERMANENTLY:
					return moved_permanently;
				case Reply::MOVED_TEMPORARILY:
					return moved_temporarily;
				case Reply::NOT_MOTIFIED:
					return not_modified;
				case Reply::BAD_REQUEST:
					return bad_request;
				case Reply::UNAUTHORIZED:
					return unauthorized;
				case Reply::FORBIDDEN:
					return forbidden;
				case Reply::NOT_FOUND:
					return not_found;
				case Reply::INTERNAL_SERVER_ERROR:
					return internal_server_error;
				case Reply::NOT_IMPLEMENTED:
					return not_implemented;
				case Reply::BAD_GATEWAY:
					return bad_gateway;
				case Reply::SERVICE_UNAVAILABLE:
					return service_unavailable;
				default:
					return internal_server_error;
				}
			}

		} // namespace stock_replies


		Reply* Reply::StockReply(StatusType eStatusType)
		{
			Reply* pReply = new Reply();
			
			pReply->SetStatus(eStatusType);
			pReply->SetContent(StockReplies::ToString(eStatusType));

			Header* pHeader1 = new Header();
			pHeader1->SetName("Content-Length");
			pHeader1->SetValue(std::to_string(pReply->GetContent().size()));

			Header* pHeader2 = new Header();
			pHeader2->SetName("Content-Type");
			pHeader2->SetValue("text / html");
			
			pReply->AddHeader(pHeader1);
			pReply->AddHeader(pHeader2);

			return pReply;
		}

		void Reply::ClearHeaders()
		{
			for (size_t index = 0; index < m_vecHeaders.size(); ++index)
			{
				Header* pHeader = m_vecHeaders[index];
				if (pHeader)
				{
					delete(pHeader);
					pHeader = NULL;
				}
			}

			m_vecHeaders.clear();
		}
	}
}