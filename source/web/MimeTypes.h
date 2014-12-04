////////////////////////////////////////////////
// File:	MimeTypes.h
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

#ifndef _MIMETYPES_H_
#define _MIMETYPES_H_

#include <string>

namespace Http
{
	namespace Server
	{
		namespace MimeTypes
		{
			std::string ExtensionToType(const std::string& szExtension);
		}
	}
}

#endif