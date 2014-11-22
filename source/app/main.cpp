////////////////////////////////////////////////
// File:	main.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright: 2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////
//#include <vld.h>

#include "Application.h"

void Exit();

Application* myApp;

int main(int argc, char *argv[])
{
	atexit(Exit);

	std::map<std::string, std::string> mapArgs;
	if (argc >= 2)
	{
		std::string currArg = "";
		std::string currVal = "";
		for (int index = 1; index < argc; ++index)
		{
			std::string strArg = argv[index];

			if (strArg.find("--") != 0 && strArg.find("-") != 0)
			{
				if (currVal.compare("") == 0)
				{
					currVal += strArg;
				}
				else
				{
					currVal += " " + strArg;
				}

				if (index == argc - 1)
				{
					mapArgs[currArg] = currVal;
				}
			}
			else
			{
				if (currArg.compare("") == 0)
				{
					currArg = strArg;
				}
				else
				{
					mapArgs[currArg] = currVal;
					currArg = strArg;
					currVal = "";
				}
			}
		}

		if (mapArgs.size() == 0 && (currArg.compare("") != 0 && currVal.compare("") != 0))
		{
			mapArgs[currArg] = currVal;
		}
	}

	myApp = Application::GetInstance();
	myApp->Initialize(mapArgs);

	std::string key;
	std::cin >> key;

	myApp->Shutdown();

	delete(myApp);
	myApp = NULL;
}

void Exit()
{
	if (myApp)
	{
		myApp->Shutdown();
		delete(myApp);
		myApp = NULL;
	}
}