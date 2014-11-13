////////////////////////////////////////////////
// File:	UIData.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////


#include "UIData.h"
#include "Section.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//Constructor
///////////////////////////////////////////////////////////////////////////////
UIData::UIData()
{
}

UIData::UIData(const char* szFileName, size_t numGPUS)
{
	ifstream myfile (szFileName);
	if(myfile.is_open())
	{
		std::string szline;
		while (getline(myfile,szline))
		{
			if(szline.find("<section") != std::string::npos)
			{
				Section* section = new Section();
				std::vector<std::string> sectionLines;

				std::string secLine;
				while (getline(myfile,secLine))
				{
					if(secLine.find("title") != std::string::npos)
					{
						size_t start = secLine.find(">") + 1;
						size_t end = secLine.find("</");

						std::string sectionTitle = secLine.substr(start, end - start);

						section->SetTitle(sectionTitle);
					}
					else if(secLine.find("<text") != std::string::npos)
					{
						std::string lineText;
						while (getline(myfile,lineText))
						{
							if(lineText.find("line") != std::string::npos)
							{
								size_t start = lineText.find(">") + 1;
								size_t end = lineText.find("</");

								std::string lineData = lineText.substr(start, end - start);
								sectionLines.push_back(lineData);
							}
							else if(lineText.find("</text") != std::string::npos)
							{
								break;
							}
						}
					}
					else if(secLine.find("</section") != std::string::npos)
					{
						break;
					}					
				}
				section->SetLines(sectionLines);
				m_vecSections.push_back(section);
			}
		}
		myfile.close();
	}

	for(size_t sectionIndex = 0; m_vecSections.size(); ++sectionIndex)
	{
		if(m_vecSections[sectionIndex]->GetTitle().compare("Devices") == 0)
		{
			std::vector<std::string> lines = m_vecSections[sectionIndex]->GetLines();
			std::vector<std::string> newLines;

			if(lines.size() > 0)
			{
				for(size_t gpuIndex = 0; gpuIndex < numGPUS; ++gpuIndex)
				{
					std::string lineStr = lines[0];

					std::stringstream ss;
					ss << "GPUID_" << gpuIndex;
					replace(lineStr, "GPUID", ss.str());

					std::stringstream ss2;
					ss2 << "GPUNAME_" << gpuIndex;
					replace(lineStr, "GPUNAME", ss2.str());

					std::stringstream ss3;
					ss3 << "HASHES_" << gpuIndex;
					replace(lineStr, "HASHES", ss3.str());

					std::stringstream ss4;
					ss4 << "TEMP_" << gpuIndex;
					replace(lineStr, "TEMP", ss4.str());

					std::stringstream ss5;
					ss5 << "GPUACCEPTED_" << gpuIndex;
					replace(lineStr, "GPUACCEPTED", ss5.str());

					std::stringstream ss6;
					ss6 << "GPUREJECTED_" << gpuIndex;
					replace(lineStr, "GPUREJECTED", ss6.str());

					std::stringstream ss7;
					ss7 << "GPUHW_" << gpuIndex;
					replace(lineStr, "GPUHW", ss7.str());

					std::stringstream ss8;
					ss8 << "GPUDIFF_" << gpuIndex;
					replace(lineStr, "GPUDIFF", ss8.str());

					newLines.push_back(lineStr);
				}
				lines.clear();
				m_vecSections[sectionIndex]->SetLines(newLines);
			}

			break;
		}
	}

}


///////////////////////////////////////////////////////////////////////////////
//Copy Constructor
///////////////////////////////////////////////////////////////////////////////
UIData::UIData(const UIData& data)
{
}

///////////////////////////////////////////////////////////////////////////////
//Assignment Operator
///////////////////////////////////////////////////////////////////////////////
UIData& UIData::operator=(const UIData& data)
{
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
//Destructor
///////////////////////////////////////////////////////////////////////////////
UIData::~UIData()
{
	for(size_t index = 0; index < m_vecSections.size(); ++index)
	{
		if(m_vecSections[index])
		{
			delete(m_vecSections[index]);
		}
	}

	m_vecSections.clear();
}

void UIData::SetParameter(std::string szName, std::string szData)
{
	m_mapParams[szName] = szData;
}


std::vector<std::string> UIData::GetLines()
{
	std::vector<std::string> lineData;

	std::string paramStart = "{@";
	std::string paramEnd = "}";

	for(size_t index = 0; index < m_vecSections.size(); ++index)
	{
		lineData.push_back(m_vecSections[index]->GetTitle());
		lineData.push_back("--------------------------------------------------------");

		std::vector<std::string> lines = m_vecSections[index]->GetLines();
		for(size_t sectionIndex = 0; sectionIndex < lines.size(); ++sectionIndex)
		{
			std::string szLineSection = lines[sectionIndex];
			
			std::string dispLine = "";
			size_t paramInd = szLineSection.find(paramStart);
			size_t paramEndInd = 0;

			size_t currentInd = 0;
			while(paramInd != std::string::npos)
			{			
				paramEndInd = szLineSection.find(paramEnd, paramEndInd);
				if(paramEndInd == std::string::npos)
				{
					break;
				}

				std::string paramName = szLineSection.substr(paramInd + 2, (paramEndInd - paramInd) - 2);
				std::string paramValue;

				std::map<std::string,std::string>::iterator it = m_mapParams.find(paramName);
				if(it != m_mapParams.end())
				{
					paramValue = it->second;
				}
				else
				{
					paramValue = paramName;
				}

				std::string displayStr = szLineSection.substr(currentInd, paramInd - currentInd);
				dispLine += displayStr + paramValue;

				currentInd += displayStr.length() + paramName.length() + 3;

				paramInd += 1;
				paramEndInd += 1;
				paramInd = szLineSection.find(paramStart, paramInd);
			}

			if(currentInd < szLineSection.length())
			{
				dispLine += szLineSection.substr(currentInd, szLineSection.length() - currentInd);
			}
			lineData.push_back(dispLine);
		}
	}

	return lineData;
}

bool UIData::replace(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}