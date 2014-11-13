////////////////////////////////////////////////
// File:	JSONUtils.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "JSONUtils.h"
#include <jansson.h>

json_t * Parse_JSON(std::string jsonStr)
{
	json_t* data;
	json_error_t error;

	std::string parseDta;

	for(int index = 0; index < jsonStr.size(); ++index)
	{
		if(jsonStr[index] != 'Ì')
		{
			parseDta += jsonStr[index];
		}
	}

	data = json_loads(parseDta.c_str(), 0, &error);

	if(!data)
	{
		return NULL;
	}

	return data;
}