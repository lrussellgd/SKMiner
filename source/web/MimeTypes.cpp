////////////////////////////////////////////////
// File:	MimeTypes.cpp
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

#include "MimeTypes.h"

namespace Http
{
	namespace Server
	{
		namespace MimeTypes
		{
			struct Mapping
			{
				const char* extension;
				const char* mimeType;
			} mappings[] =
			{
				{	"gif", "image/gif"			},
				{	"htm", "text/html"			},
				{	"html", "text/html"			},
				{	"jpg", "image/jpeg"			},
				{	"png", "image/png"			},
				{	"json", "application/json"	}
			};

			std::string ExtensionToType(const std::string& szExtension)
			{
				for (Mapping m : mappings)
				{
					if (m.extension == szExtension)
					{
						return m.mimeType;
					}
				}

				return "text/plain";
			}
		}
	}
}