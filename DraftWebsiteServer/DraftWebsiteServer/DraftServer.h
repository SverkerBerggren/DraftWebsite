#pragma once
#include "DraftWebsiteServer.h"
#include "httplib.h"
#include "Lobby.h"

class DraftServer
{
private: 

    void LoadAvailableCards();

    int lobbyId = 0;
    int playerCookieId = 0;

    std::mutex serverMutex = std::mutex();
    httplib::Server svr;

    std::unordered_map<std::string, std::string> playerToLobby;

    std::unordered_map<std::string, Lobby> activeLobbies;

    std::vector<std::string> availableCards;


    void RemoveLobby(const std::string& lobbyId);

    std::string HostLobby(const std::string& playerId);
    
public: 


    void Start();
};