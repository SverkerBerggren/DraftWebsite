// DraftWebsiteServer.cpp : Defines the entry point for the application.
//


#include "DraftWebsiteServer.h"
#include "httplib.h"
#include "Lobby.h"
#include "DraftServer.h"
using namespace httplib;


int main()
{
	DraftServer draftServer;

	draftServer.Start();

	return 0;
}
