////////////////////////////////////////////////
// File:	Inbound.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Inbound.h"
#include "Packet.h"
#include "types.h"
#include "DDOS_Filter.h"

namespace LLP
{
	Inbound::Inbound() : Connection()
	{
	}

	Inbound::Inbound(LLP::Socket_t* SOCKET_IN, LLP::DDOS_Filter* DDOS_IN) : Connection(SOCKET_IN, DDOS_IN)
	{

	}

	Inbound::Inbound(const Inbound& inbound)
	{
	}

	Inbound& Inbound::operator = (const Inbound& inbound)
	{
		return *this;
	}

	Inbound::~Inbound()
	{
	}

	void Inbound::Start()
	{
		m_vecReadData.clear();
		INCOMING->SetNull();

		AsyncRead();
	}

	void Inbound::AsyncRead()
	{
		if (Errors())
			return;

		if (!SOCKET)
		{
			return;
		}

		m_vecReadData.clear();

		if (SOCKET->get()->available() > 0 && INCOMING->GetHeader() == 255)
		{
			boost::asio::async_read(*this->SOCKET->get(),
				boost::asio::buffer(m_vecReadData, 1),
				boost::bind(&Inbound::HandleHeader, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

			return;
		}

		if (INCOMING->GetHeader() != 255 && !INCOMING->Complete())
		{
			if (SOCKET->get()->available() >= 4 && INCOMING->GetLength() == 0)
			{
				boost::asio::async_read(*this->SOCKET->get(),
					boost::asio::buffer(m_vecReadData, 4),
					boost::bind(&Inbound::HandleLength, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

				return;
			}

			unsigned int nAvailable = (unsigned int)SOCKET->get()->available();
			if (nAvailable > 0 && INCOMING->GetLength() > 0 && INCOMING->GetData().size() < INCOMING->GetLength())
			{
				m_vecReadData.resize(std::min(nAvailable, (unsigned int)(INCOMING->GetLength() - INCOMING->GetData().size())));

				boost::asio::async_read(*this->SOCKET->get(),
					boost::asio::buffer(m_vecReadData, m_vecReadData.size()),
					boost::bind(&Inbound::HandleRead, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

				return;
			}
		}
	}

	void Inbound::HandleHeader(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			return;
		}

		if (bytes_transferred == 1)
		{
			INCOMING->SetHeader(m_vecReadData[0]);
		}

		AsyncRead();
	}

	void Inbound::HandleLength(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			return;
		}

		if (bytes_transferred == 4)
		{
			INCOMING->SetLength(m_vecReadData);
		}

		AsyncRead();
	}

	void Inbound::HandleRead(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			return;
		}

		if (bytes_transferred == m_vecReadData.size())
		{
			std::vector<unsigned char> dta = INCOMING->GetData();
			try
			{
				dta.insert(dta.end(), m_vecReadData.begin(), m_vecReadData.end());
			}
			catch (const std::exception e)
			{
				return;
			}
			INCOMING->SetData(dta);

			Event(bytes_transferred);
		}
	}
}