////////////////////////////////////////////////
// File:	StringUtils.h
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#ifndef _STRINGUTILS_H_
#define _STRINGUTILS_H_

#include <string>
#include <vector>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

#endif //_STRINGUTILS_H_