#pragma once
#include "DraftWebsiteServer.h"
#include "httplib.h"
#include "Lobby.h"
#include <chrono>
// dessa beskrivs i cpp filen
class DraftServer
{
private: 

    void LoadAvailableCards();
    void RemoveInactiveLobbies();

    int lobbyId = 0;
    int playerCookieId = 0;

    std::mutex serverMutex = std::mutex();
    httplib::Server svr;

    std::unordered_map<std::string, std::string> playerToLobby;

    std::unordered_map<std::string, Lobby> activeLobbies;

    std::vector<std::string> availableCards;

    std::string pointOfEntry;

    void RemoveLobby(const std::string& lobbyId);

    std::string HostLobby(const std::string& playerId);

    int maxRequestsBeforeLobbyDestroy = 45;
    int currentRequestsBeforeDestroy = 0; 
    
    int minutesBeforeDestruction = 15; 

   // std::unordered_map < std::string, std::chrono::system_clock> lobbyToTimeStamp;  
public: 


    void Start(const std::string& entryPoint);
};