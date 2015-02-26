////////////////////////////////////////////////
// File:	FileLoader.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "FileLoader.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

FileLoader::FileLoader()
{
	this->m_enmEntityType = ENTITY_TYPE::FILE_LOADER;

	boost::filesystem::path dir(APP_DATA_PATH);
	if (!boost::filesystem::exists(dir))
	{
		if (!boost::filesystem::create_directory(dir))
		{
			std::cout << "Could NOT create App Data Directory at " << APP_DATA_PATH << std::endl;
		}
	}
}

FileLoader::~FileLoader()
{

}

bool FileLoader::Save(const std::string& szFileName, const std::string& szData, const std::string& szSubDirectory, bool bOverwrite, bool bIsBinary)
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
					std::ofstream strFile(szDirFile.c_str());
					if (strFile.is_open())
					{
						strFile << szData;
						strFile.close();
						bSuccess = true;
					}
				}
				else
				{
					std::ofstream strFile(szDirFile, std::ios::binary);
					if (strFile.is_open())
					{
						strFile << szData;
						strFile.close();
						bSuccess = true;
					}
				}
			}
		}
	}

	return bSuccess;
}

bool FileLoader::Load(const std::string& szFileName, std::string& szData, const std::string& szSubDirectory, bool bIsBinary)
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
				std::string readVal;

				if (!bIsBinary)
				{
					std::ifstream strFile(szDirFile);
					if (strFile.is_open())
					{
						std::string szLine;
						while (std::getline(strFile, szLine))
						{
							readVal += szLine;
						}

						strFile.close();

						szData = readVal;
						bSuccess = true;
					}
				}
				else
				{
					
					std::streampos size;
					char* pDataBlock;

					std::ifstream strFile(szDirFile, std::ios::binary);
					if (strFile.is_open())
					{
						size = strFile.tellg();
						pDataBlock = new char[size];

						strFile.seekg(0, std::ios::beg);
						strFile.read(pDataBlock, size);
						strFile.close();

						std::string readVal(pDataBlock);
						szData = readVal;

						delete(pDataBlock);
						bSuccess = true;
					}
				}				
			}
		}
	}

	return bSuccess;
}