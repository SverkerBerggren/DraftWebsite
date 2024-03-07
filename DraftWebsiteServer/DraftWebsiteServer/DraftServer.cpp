#include "DraftServer.h"
#include <filesystem>
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
    svr.Get("/DraftableCards", [&](const httplib::Request& req, Response& res) {

        std::string playerId = req.get_header_value("Cookie");

        {
            std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
            if (playerToLobby.find(playerId) != playerToLobby.end())
            {
                res.set_content(activeLobbies[playerToLobby[playerId]].GetDraftableCardsPlayer(playerId), "text/plain");
            }
        }


        });
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

    std::function<void(const Request&, Response&)> hostLobbyFunction = [&](const Request&, Response&) {&DraftServer::HostLobbyRequest; };
  //  svr.Post("/HostLobby", hostLobbyFunction);

    svr.Post("/HostLobby", [&](const httplib::Request& req, Response& res) {
  
            std::string playerId = req.get_header_value("Cookie");
  
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

    svr.Post("/JoinLobby", [&](const httplib::Request& req, Response& res) {
        std::string playerId = req.get_header_value("Cookie");
        if (activeLobbies.find(req.body) != activeLobbies.end())
        {
            res.set_content("Accepted", "text/plain");
            
           std::string lobbyId = req.body;
           playerToLobby[playerId] = req.body;
           activeLobbies[req.body].AddConnectedPlayer(playerId);
        }
        else
        {
            res.set_content("nej", "text/plain");
        }
        
        std::cout << "join grejen " <<playerId  << std::endl;
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

        if (index != -1)
        {
            activeLobbies[playerToLobby[playerId]].PickCard(playerId, index);
        }


        std::cout << "pick grejen " << playerId << std::endl;
        });

    svr.Get("/hi", [](const httplib::Request& req, Response& res) {
        std::cout << "hej " << std::endl;
        res.status = StatusCode::Accepted_202;
        res.set_content("har var det data", "text/plain");
        });

    svr.listen("localhost", 1234);
}

void DraftServer::LoadAvailableCards()
{
    std::filesystem::path imagesPath = std::filesystem::path("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteServer\\CardImages");

    for (auto const& card : std::filesystem::directory_iterator{imagesPath})
    {
        availableCards.push_back(card.path().filename().string());

        std::cout << card.path().filename().string();
    }
}



std::string DraftServer::HostLobby(std::string playerId)
{   


     
    activeLobbies[std::to_string(lobbyId)] = Lobby(playerId, 3, 3);
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