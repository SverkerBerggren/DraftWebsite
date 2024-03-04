#pragma once
#include "DraftWebsiteServer.h"
#include "httplib.h"
#include "Lobby.h"

class DraftServer
{
private: 

    void LoadAvailableCards();
    
public: 
    httplib::Server svr;

    std::unordered_map<std::string, Lobby> playerToLobby;

    std::unordered_map<std::string, Lobby> activeLobbies;

    std::vector<std::string> availableCards; 

    void Start();
};