// DraftWebsiteServer.cpp : Defines the entry point for the application.
//


#include "DraftWebsiteServer.h"
#include "httplib.h"
#include "Lobby.h"
#include "DraftServer.h"
using namespace httplib;

//main funktionen. Argumentet man skickar in fungerar som entrypointen för de sparade resurserna 
int main(int argc, char* argv[])
{
	DraftServer draftServer;
	std::vector<std::string> ConvertedArgv;
	for (int i = 1; i < argc; i++)
	{
		ConvertedArgv.push_back(argv[i]);
	}
	if (argc == 1)
	{
		
		draftServer.Start("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML",ConvertedArgv);
	}
	else
	{	
		draftServer.Start( std::string(argv[1]),ConvertedArgv);
	}

	return 0;
}
