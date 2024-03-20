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
	if (argc == 1)
	{
		draftServer.Start("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML");
	}
	else
	{	
		if (argc == 2)
		{
			draftServer.Start( std::string(argv[1]));

		}
	}

	return 0;
}
