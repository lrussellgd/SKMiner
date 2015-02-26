////////////////////////////////////////////////
// File:	ConfigConnection.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "ConfigConnection.h"

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
ConfigConnection::ConfigConnection()
{
	this->m_enmEntityType = ENTITY_TYPE::CONFIG_CONNECTION;
}

///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
ConfigConnection::ConfigConnection(const ConfigConnection& conn) : Entity(conn)
{
	this->m_szURL = conn.GetURL();
	this->m_szUser = conn.GetUser();
	this->m_szPassword = conn.GetPassword();
}


ConfigConnection& ConfigConnection::operator=(const ConfigConnection& conn)
{
	this->m_enmEntityType = conn.GetEntityType();
	this->m_szURL = conn.GetURL();
	this->m_szUser = conn.GetUser();
	this->m_szPassword = conn.GetPassword();

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
ConfigConnection::~ConfigConnection()
{
}

ConfigConnection* ConfigConnection::Clone()
{
	ConfigConnection* confConn = new ConfigConnection();

	confConn->SetURL(this->m_szURL);
	confConn->SetUser(this->m_szUser);
	confConn->SetPassword(this->m_szPassword);

	return confConn;
}

ConfigConnection* ConfigConnection::DeepCopy()
{
	ConfigConnection* confConn = new ConfigConnection();

	confConn->SetURL(this->m_szURL);
	confConn->SetUser(this->m_szUser);
	confConn->SetPassword(this->m_szPassword);

	return confConn;
}
