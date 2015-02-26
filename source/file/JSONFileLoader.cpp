////////////////////////////////////////////////
// File:	JSONFileLoader.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "JSONFileLoader.h"
#include <fstream>
#include <boost/filesystem.hpp>

JSONFileLoader::JSONFileLoader()
{
	this->m_enmEntityType = ENTITY_TYPE::JSON_FILE_LOADER;

	boost::filesystem::path dir(APP_DATA_PATH);
	if (!boost::filesystem::exists(dir))
	{
		if (!boost::filesystem::create_directory(dir))
		{
			std::cout << "Could NOT create App Data Directory at " << APP_DATA_PATH << std::endl;
		}
	}
}

JSONFileLoader::~JSONFileLoader()
{

}

bool JSONFileLoader::Save(const std::string& szFileName, json_spirit::mObject tData, const std::string& szSubDirectory, bool bOverwrite, bool bIsBinary)
{
	bool bSuccess = false;

	if (!szFileName.empty())
	{
		std::string szDirPath = APP_DATA_PATH + "\\";
		if (!szSubDirectory.empty())
		{
			szDirPath += szSubDirectory + "\\";
			boost::filesystem::path newDir(szDirPath);

			if (!boost::filesystem::exists(newDir))
			{
				if (!boost::filesystem::create_directory(newDir))
				{
					bSuccess = false;
				}
			}
		}

		if (!szDirPath.empty())
		{
			std::string szDirFile = szDirPath + szFileName;
			boost::filesystem::path dirFile(szDirFile);
			if ((!boost::filesystem::exists(dirFile)) || (bOverwrite && boost::filesystem::is_regular_file(dirFile)))
			{
				if (!bIsBinary)
				{
					std::ofstream jsonFile(szDirFile);
					json_spirit::write(tData, jsonFile, json_spirit::Output_options::pretty_print);
					jsonFile.close();
					
				}
				else
				{
					std::ofstream jsonBinaryFile(szDirFile, std::ios::binary);
					json_spirit::write(tData, jsonBinaryFile, json_spirit::Output_options::none);
					jsonBinaryFile.close();
				}

				bSuccess = true;
			}
		}
	}

	return bSuccess;
}

bool JSONFileLoader::Load(const std::string& szFileName, json_spirit::mObject& tData, const std::string& szSubDirectory, bool bIsBinary)
{
	bool bSuccess = false;

	if (!szFileName.empty())
	{
		std::string szDirPath = APP_DATA_PATH + "\\";
		if (!szSubDirectory.empty())
		{
			szDirPath += szSubDirectory + "\\";
			boost::filesystem::path newDir(szDirPath);

			if (!boost::filesystem::exists(newDir))
			{
				if (!boost::filesystem::create_directory(newDir))
				{
					bSuccess = false;
				}
			}
		}

		if (!szDirPath.empty())
		{
			std::string szDirFile = szDirPath + szFileName;
			boost::filesystem::path dirFile(szDirFile);
			if (boost::filesystem::exists(dirFile))
			{
				json_spirit::mValue readVal;

				if (!bIsBinary)
				{	
					std::ifstream jsonInFile(szDirFile);
					json_spirit::read(jsonInFile, readVal);
					jsonInFile.close();
				}
				else
				{
					std::ifstream jsonInFile(szDirFile, std::ios::binary);
					json_spirit::read(jsonInFile, readVal);
					jsonInFile.close();
				}

				tData["DATA"] = readVal;
				bSuccess = true;
			}
		}
	}

	return bSuccess;
}