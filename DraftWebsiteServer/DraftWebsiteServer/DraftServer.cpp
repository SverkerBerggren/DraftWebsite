#include "DraftServer.h"
#include <filesystem>
#include <thread>
#include <chrono>
#include <condition_variable>
#include "json.hpp"

using namespace httplib;
using json = nlohmann::json;
/// <summary>
/// servern som startas från main loopen 
/// </summary>
/// <param name="entryPoint"></param>
void DraftServer::Start(const std::string& entryPoint)
{

    pointOfEntry = entryPoint;
    svr.set_mount_point("/", pointOfEntry);

    LoadAvailableCards();
    //Sätter alla http requests 

    //REquesten för att få alla resurser som css och bilder och dylikt
    svr.Get("/", [&](const httplib::Request& req, Response& res) {
        std::cout << "hej getta html" << std::endl;
        std::cout << " Cookie " << req.get_header_value("Cookie") << std::endl;
        std::string tempString = pointOfEntry;
        std::ifstream ifStream = std::ifstream(tempString.append("\\placeholder.html"));

        ifStream.seekg(0, ifStream.end);
        std::string html = std::string(ifStream.tellg(), 0);
        ifStream.seekg(0, ifStream.beg);
        ifStream.read(html.data(), html.size());
        res.set_content(html, "text/html");
        if (req.get_header_value("Cookie") == "")
        {
            res.set_header("Set-Cookie", "PlayerId = " + std::to_string(playerCookieId) + "; Expires=Thu, 21 Apr 2024 07:28:00 GMT;");
        }
        playerCookieId += 1;
        std::cout << html.size() << std::endl;
        });


    //requesten som behandlar alla pollande uppdateringar från klienten. Avgör requesten utifrån bodyn och returnar utefter det korrosponerande json fil
    svr.Post("/Update", [&](const httplib::Request& req, Response& res) {

        //std::string stringToReturn = "";
        std::string playerId = req.get_header_value("Cookie");
        json message;

        std::string updateType = req.body;
        {
            std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
            auto iterator = playerToLobby.find(playerId);

            if (iterator != playerToLobby.end())
            {   
                if (updateType._Equal("UpdateDraftableCards"))
                {
                    message["DraftableCards"] = activeLobbies[iterator->second].GetDraftableCardsPlayer(playerId);
                    message["DraftFinished"] = activeLobbies[iterator->second].IsDraftFinished();

                }   
                else if (updateType._Equal("ConnectedPlayers"))
                {
                    message["ConnectedPlayers"] = activeLobbies[iterator->second].GetConnectedPlayers();
                }
                else if (updateType._Equal("HasLobbyStarted"))
                {
                    message["HasLobbyStarted"] = activeLobbies[iterator->second].HasLobbyStarted();
                }
            }
        }
        
        std::cout << "update grejen " << updateType << std::endl;
        //std::string debugString = message.dump();
        res.set_content(message.dump(), "text/plain");

        });

    //Requesten som behandlar när en spelare vill se kortet de har tagit digiare
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

    //Requesten som behandlar när en spelare vill hosta en lobby
    svr.Post("/HostLobby", [&](const httplib::Request& req, Response& res) {
  
            std::string playerId = req.get_header_value("Cookie");
            json message;
            bool shouldHostLobby = false;
            {
                std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
                auto iterator = playerToLobby.find(playerId);
                if (iterator == playerToLobby.end())
                {
                    shouldHostLobby = true;
                }

            }
            if (shouldHostLobby)
            {   
                message["Accepted"] = true; 
                message["LobbyId"] = HostLobby(playerId);
                res.set_content(message.dump(), "text/plain");
            }
            else
            {
                message["Accepted"] = false;

                res.set_content(message.dump(), "text/plain");
            }
            
            std::cout << "host grejen " << playerId << std::endl;
        });
    //Requesten som behandlar när en spelare vill starta lobbyn. Går bara om man är spelaren som hostade lobbyn
    svr.Post("/StartLobby", [&](const httplib::Request& req, Response& res) {

        std::string playerId = req.get_header_value("Cookie");
        std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
        auto iterator = activeLobbies.find(playerToLobby[playerId]);
        if (iterator != activeLobbies.end())
        {
            if (playerId._Equal(iterator->second.GetHost()) && !iterator->second.HasLobbyStarted())   
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
    //Requesten som behandlar när en spelare vill gå med en lobby. Man kan bara vara med i en lobby samtidigt

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
                    if (iterator->second.IsPlayerConnected(playerId))
                    {
                        res.set_content("Already connected", "text/plain");

                    }
                    else
                    {
                        playerToLobby[playerId] = req.body;
                        iterator->second.AddConnectedPlayer(playerId);
                        res.set_content("Accepted", "text/plain");
                    }

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

    //metoden när en spelare vill ta ett kort som är tillgängligt till dom 
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

//laddar in korten för bilderna utifrån en lokal resurs
void DraftServer::LoadAvailableCards()
{
    std::string tempstring = pointOfEntry;
    std::filesystem::path imagesPath = std::filesystem::path(tempstring.append("\\CardImages"));

    for (auto const& card : std::filesystem::directory_iterator{imagesPath})
    {
        availableCards.push_back(card.path().filename().string());

        std::cout << card.path().filename().string();
    }
}


//metoden för när man hosta en lobby
std::string DraftServer::HostLobby(const std::string& playerId)
{   

    std::lock_guard<std::mutex> lockGuard(serverMutex);
     
    activeLobbies[std::to_string(lobbyId)] = Lobby(playerId, 3, 3);
//    activeLobbies[std::to_string(lobbyId)].StartLobby(availableCards);
    playerToLobby[playerId] =  std::to_string(lobbyId);
    lobbyId += 1;
    std::string stringToReturn = std::to_string(lobbyId -1);
    return stringToReturn;
}
//ofärdig metod för när man ska ta bort en lobby
void DraftServer::RemoveLobby(const std::string& lobbyId)
{   
    std::lock_guard<std::mutex> lockGuard(serverMutex);

    auto activeLobbiesIterator = activeLobbies.find(lobbyId);
    if(activeLobbiesIterator != activeLobbies.end())
    {
        activeLobbies.erase(activeLobbiesIterator);
    }
    
    std::vector<std::string> playersToRemove;

    for (std::pair<const std::string, std::string> playerToRemove : playerToLobby)
    {
        if (playerToRemove.second._Equal(lobbyId))
        {
            playersToRemove.push_back(playerToRemove.first);
        }
    }

    for (std::string playerToRemove : playersToRemove)
    {
        playerToLobby.erase(playerToRemove);
    }

}
