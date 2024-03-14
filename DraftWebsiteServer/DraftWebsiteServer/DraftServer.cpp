#include "DraftServer.h"
#include <filesystem>
#include <thread>
#include <chrono>
#include <condition_variable>

using namespace httplib;
void DraftServer::Start()
{
    svr.set_mount_point("/", "D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML");

    LoadAvailableCards();

    svr.Get("/", [&](const httplib::Request& req, Response& res) {
        std::cout << "hej getta html" << std::endl;
        std::cout << " Cookie " << req.get_header_value("Cookie") << std::endl;

        std::ifstream ifStream = std::ifstream("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML\\placeholder.html");

        ifStream.seekg(0, ifStream.end);
        std::string html = std::string(ifStream.tellg(), 0);
        ifStream.seekg(0, ifStream.beg);
        ifStream.read(html.data(), html.size());
        res.set_content(html, "text/html");
        if (req.get_header_value("Cookie") == "")
        {
            res.set_header("Set-Cookie", "PlayerId=" + std::to_string(playerCookieId) + "; Expires=Thu, 21 Apr 2024 07:28:00 GMT;");
        }
        playerCookieId += 1;
        std::cout << html.size() << std::endl;
        });

    //ska tas bort
    svr.Get("/AvailableCards", [&](const httplib::Request& req, Response& res) {

        std::string stringToReturn = "";

        for (int i = 0; i < availableCards.size(); i++)
        {
            stringToReturn += availableCards[i];
            if (i != availableCards.size() - 1)
            {
                stringToReturn += ":";
            }
        }
        res.status = StatusCode::OK_200;
        std::cout << "put grejen " << stringToReturn << std::endl;
        res.set_content(stringToReturn, "text/plain");
        });

    svr.Get("/Update", [&](const httplib::Request& req, Response& res) {

        std::string stringToReturn = "DraftableCards:";
        std::string playerId = req.get_header_value("Cookie");

        {
            std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
            auto iterator = playerToLobby.find(playerId);
            if (iterator != playerToLobby.end())
            {
                if (activeLobbies[iterator->second].IsDraftFinished())
                {
                    stringToReturn = "DraftFinished";
                }
                else
                {
                    stringToReturn += activeLobbies[iterator->second].GetDraftableCardsPlayer(playerId);
                }
            }
        }
        
        std::cout << "update grejen " << stringToReturn << std::endl;
        res.set_content(stringToReturn, "text/plain");

        });

    svr.Get("/PickedCards", [&](const httplib::Request& req, Response& res) {

        std::string stringToReturn = "";
        std::string playerId = req.get_header_value("Cookie");

        {
            std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
            auto iterator = playerToLobby.find(playerId);
            if (iterator != playerToLobby.end())
            {
                stringToReturn += activeLobbies[iterator->second].GetPickedCardsPlayer(playerId);
            }
        }

        std::cout << "draftedCards grejen " << stringToReturn << std::endl;
        res.set_content(stringToReturn, "text/plain");

        });

    std::function<void(const Request&, Response&)> hostLobbyFunction = [&](const Request&, Response&) {&DraftServer::HostLobbyRequest; };
  //  svr.Post("/HostLobby", hostLobbyFunction);

    svr.Post("/HostLobby", [&](const httplib::Request& req, Response& res) {
  
            std::string playerId = req.get_header_value("Cookie");
            std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);

            
            if (playerToLobby.find(playerId) == playerToLobby.end())
            {
  
                res.set_content(HostLobby(playerId), "text/plain");

            }
            else
            {
                res.set_content("redan i lobby", "text/plain");

            }
  
            std::cout << "host grejen " << playerId << std::endl;
        });

    svr.Post("/StartLobby", [&](const httplib::Request& req, Response& res) {

        std::string playerId = req.get_header_value("Cookie");
        std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
        auto iterator = activeLobbies.find(playerToLobby[playerId]);
        if (iterator != activeLobbies.end())
        {
            if (playerId._Equal(iterator->second.GetHost()))
            {
                iterator->second.StartLobby(availableCards);
                res.set_content("lobby started", "text/plain");
            }
            res.set_content("du ar inte host", "text/plain");
        }
        else
        {
            res.set_content("lobby finns inte", "text/plain");
        }
        std::cout << "start grejen " << playerId << std::endl;
        });

    svr.Post("/JoinLobby", [&](const httplib::Request& req, Response& res) {    
        std::string playerId = req.get_header_value("Cookie");
        std::string lobbyId = req.body;
        std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
        {
            auto iterator = activeLobbies.find(lobbyId);
            if (iterator != activeLobbies.end())
            {
                if (!iterator->second.HasLobbyStarted())
                {
                    playerToLobby[playerId] = req.body;
                    iterator->second.AddConnectedPlayer(playerId);
                    res.set_content("Accepted", "text/plain");
                }
                else
                {
                    res.set_content("nej", "text/plain");
                }
            }
            else
            {
                res.set_content("nej", "text/plain");
            }
        }
        std::cout << "join grejen " <<playerId  << std::endl;
        });


    svr.Post("/UpdatePlayers", [&](const httplib::Request& req, Response& res) {
        std::string playerId = req.get_header_value("Cookie");
        std::string lobbyId = req.body;
        {
            std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
            auto iterator = activeLobbies.find(playerToLobby[playerId]);
            if (iterator != activeLobbies.end())
            {

                playerToLobby[playerId] = req.body;
                iterator->second.AddConnectedPlayer(playerId);
                res.set_content("Accepted", "text/plain");
                res.body = iterator->second.GetConnectedPlayers();
            }
            else
            {
                res.set_content("nej", "text/plain");
            }
        }
        std::cout << "join grejen " << playerId << std::endl;
        });

    svr.Post("/PickCard", [&](const httplib::Request& req, Response& res) {
        std::string playerId = req.get_header_value("Cookie");
        int index = -1;
        try
        {
            index = std::stoi(req.body);
        }
        catch(std::exception e){
            std::cout << e.what();
        }

        std::cout << "pick grejen " << playerId << std::endl;

        std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);

        if (index != -1)
        {
            activeLobbies[playerToLobby[playerId]].PickCard(playerId, index);
        }


        });

    svr.Get("/hi", [](const httplib::Request& req, Response& res) {
        std::cout << "hej " << std::endl;
        res.status = StatusCode::Accepted_202;
        res.set_content("har var det data", "text/plain");
        });

    svr.listen("0.0.0.0", 1234);
    
}

void DraftServer::LoadAvailableCards()
{
    std::filesystem::path imagesPath = std::filesystem::path("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML\\CardImages");

    for (auto const& card : std::filesystem::directory_iterator{imagesPath})
    {
        availableCards.push_back(card.path().filename().string());

        std::cout << card.path().filename().string();
    }
}



std::string DraftServer::HostLobby(std::string playerId)
{   


     
    activeLobbies[std::to_string(lobbyId)] = Lobby(playerId, 3, 3);
    activeLobbies[std::to_string(lobbyId)].StartLobby(availableCards);
    playerToLobby[playerId] =  std::to_string(lobbyId);
    lobbyId += 1;
    std::string stringToReturn = std::to_string(lobbyId -1);
    return stringToReturn;
}

void DraftServer::HostLobbyRequest(const httplib::Request& req, httplib::Response& res)
{

    std::string playerId = req.get_header_value("Cookie");

    if (playerToLobby.find(playerId) != playerToLobby.end())
    {

        res.set_content(HostLobby(playerId), "text/plain");
    }

    std::cout << "host grejen " << playerId << std::endl;
}