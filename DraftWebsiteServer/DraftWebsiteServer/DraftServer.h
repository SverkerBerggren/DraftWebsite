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
    
public: 
    httplib::Server svr;

    std::unordered_map<std::string, std::string> playerToLobby;

    std::unordered_map<std::string, Lobby> activeLobbies;

    std::vector<std::string> availableCards; 


    std::string HostLobby(std::string playerId);

    void HostLobbyRequest(const httplib::Request& req, httplib::Response& res);
    void JoinLobbyRequest(const httplib::Request& req, httplib::Response& res);
    void GetHTMLRequest(const httplib::Request& req, httplib::Response& res);
    void GetDraftableCardsRequest(const httplib::Request& req, httplib::Response& res);

    void Start();
};