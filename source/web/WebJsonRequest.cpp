////////////////////////////////////////////////
// File:	WebRequest.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "WebJsonRequest.h"


WebJsonRequest::WebJsonRequest()
{
	this->m_enmEntityType = ENTITY_TYPE::WEB_JSON_REQUEST;
}

	
WebJsonRequest::~WebJsonRequest()
{
}

std::string WebJsonRequest::RequestJSONString(std::string szRequestURL)
{
	http_client client(utility::conversions::to_string_t(szRequestURL));
	auto jsonVal = std::make_shared<utility::string_t>();
	pplx::task<void> requestTask = client.request(methods::GET).then([=](http_response response)
	{
		return response.extract_string();

	}).then([=](utility::string_t szRetStr)
	{
		*jsonVal = szRetStr;
		return;
	});

	try
	{
		requestTask.wait();
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
		
	std::string szVal = std::string(jsonVal->begin(), jsonVal->end());
	return szVal;
}