#pragma once
#include "DraftWebsiteServer.h"
#include "httplib.h"
#include "Lobby.h"
#include <chrono>
#include <guiddef.h>
#include "sqlite3.h"
#include "httplib.h"
// dessa beskrivs i cpp filen
using namespace httplib;

class DraftServer
{
private: 


    void JoinLobby(const Request& req, Response& res);
    void LoadAvailableCards();
    void RemoveInactiveLobbies();
    void RemoveLobby(const std::string& lobbyId);
    void GiveCookie(const Request& req, Response& res);
    
    void ServeHTML(const Request& req, Response& res, const std::string& htmlPath);
    std::string HostLobby(const std::string& playerId, int mainDeckCardsPerPack, int extraDeckCardsPerPack, int amountOfPacks);
    std::string GetUniqueLobbyURL();

    // int lobbyId = 0;
    std::mutex serverMutex = std::mutex();
    httplib::Server svr;
    std::unordered_map<std::string, std::string> playerToLobby;
    std::unordered_map<std::string, Lobby> activeLobbies;
    std::vector<std::string> availableMainDeckCards;
    std::vector<std::string> availableExtraCards;
    std::string pointOfEntry;
    int maxRequestsBeforeLobbyDestroy = 45;
    int currentRequestsBeforeDestroy = 0; 
    int minutesBeforeDestruction = 15;
    sqlite3* sqliteDataBaseHandle = nullptr;

   // std::unordered_map < std::string, std::chrono::system_clock> lobbyToTimeStamp;  
public: 


    void Start(const std::string& entryPoint);
};