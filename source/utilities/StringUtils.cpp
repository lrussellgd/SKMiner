////////////////////////////////////////////////
// File:	StringUtils.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "StringUtils.h"
#include <sstream>
#include <iomanip>

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) 
	{
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string ToHex(const int nByte)
{
	std::stringstream ssHex;
	ssHex << std::setw(2) << std::uppercase << std::hex << (unsigned int)nByte;

	return ssHex.str();
}

std::string ToHex(const char chByte)
{
	std::stringstream ssHex;
	ssHex << std::setw(2) << std::uppercase << std::hex << (unsigned int)chByte;

	return ssHex.str();
}

std::string ToHex(const std::string& szBytes)
{
	std::stringstream ssHex;
	for (size_t index = 0; index < szBytes.length(); ++index)
	{
		ssHex << std::setw(2) << std::uppercase << std::hex << (unsigned int)szBytes[index];
	}

	return ssHex.str();
}