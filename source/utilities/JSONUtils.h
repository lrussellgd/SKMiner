////////////////////////////////////////////////
// File:	JSONUtils.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#ifndef _JSONUTILS_H_
#define _JSONUTILS_H_

#include <string>

struct json_t;

json_t * Parse_JSON(std::string jsonStr);

#endif //_JSONUTILS_H_

