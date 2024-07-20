#include "DraftServer.h"
#include <filesystem>
#include <thread>
#include <chrono>
#include <condition_variable>
#include "json.hpp"
#include <rpc.h>
#include <rpcdce.h>

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
    const char* dataBasePath = "D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteServer\\UserDatabase.db";
    
    sqlite3* databaseHandle = nullptr; 

    if (sqlite3_open(dataBasePath, &databaseHandle) == SQLITE_OK)
    {

    }

    //REquesten för att få alla resurser som css och bilder och dylikt
    svr.Get("/", [&](const httplib::Request& req, Response& res) {
        std::cout << "hej getta html" << std::endl;
        std::cout << " Cookie " << req.get_header_value("Cookie") << std::endl;
        GiveCookie(req, res);
        ServeHTML(req, res, "RootSite.html");
        std::cout << "hur manga lobbies " << std::to_string(activeLobbies.size()) << std::endl;
       // std::cout << html.size() << std::endl;
        });
    svr.Get("/DraftLobby", [&](const httplib::Request& req, Response& res) {
        GiveCookie(req,res);
        //ServeHTML(req, res,"DraftLobby.html");
        std::string playerId = req.get_header_value("Cookie");
        json message;
        if (!req.has_param("LobbyId"))
        {
            res.set_content("knas", "text/plain");
            return; 
        }

        std::string lobbyId = req.get_param_value("LobbyId");

        std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
        {
            auto iterator = activeLobbies.find(lobbyId);

            if (iterator != activeLobbies.end())
            {
                if (iterator->second.IsPlayerConnected(playerId))
                {
                    //res.set_content("Already connected", "text/plain");
                    ServeHTML(req, res, "DraftLobby.html");

                }
                else
                {
                    if (iterator->second.HasLobbyStarted())
                    {
                        ServeHTML(req, res, "DraftLobby.html");

                    }
                    else
                    {
                        playerToLobby[playerId] = lobbyId;
                        iterator->second.AddConnectedPlayer(playerId);
                        //res.set_content("Accepted", "text/plain");
                        ServeHTML(req, res, "DraftLobby.html");
                    }
                }
            }
            else
            {
                res.set_content("knas", "text/plain");
            }
        }
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
                    message["IsHost"] = activeLobbies[iterator->second].GetHost() == playerId;
                    message["InviteLink"] = "http://sus.dorctown.com:1234/DraftLobby?LobbyId=" + iterator->second;
                }
                else if (updateType._Equal("HasLobbyStarted"))
                {
                    message["HasLobbyStarted"] = activeLobbies[iterator->second].HasLobbyStarted();
                }

            }
        }
        std::string messageCheck = message.dump();
     //   std::cout << "update grejen " << updateType << std::endl;
        //std::string debugString = message.dump();
        res.set_content(message.dump(), "text/plain");

        });
    svr.Post("/ReceivedDraftFinished", [&](const httplib::Request& req, Response& res) {
        std::string playerId = req.get_header_value("Cookie");
        json message;

        bool shouldRemoveLobby = false;
        std::string lobbyToRemove;
        std::string updateType = req.body;
        {
            std::lock_guard<std::mutex> lockGuard(serverMutex);
            auto iterator = playerToLobby.find(playerId);
            if (iterator != playerToLobby.end())
            {
                activeLobbies[iterator->second].UpdatePlayerSeenLobbyEnded(iterator->first);
                if (activeLobbies[iterator->second].HasLobbyEnded())
                {
                    shouldRemoveLobby = true;
                    lobbyToRemove = iterator->second;
                }
            } 
        }

        if (shouldRemoveLobby)
        {
            activeLobbies[lobbyToRemove].LoggDraftToSQL(databaseHandle);

            RemoveLobby(lobbyToRemove);
        }

        res.set_content("", "text/plain");
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

        //std::cout << "draftedCards grejen " << stringToReturn << std::endl;
        res.set_content(stringToReturn, "text/plain");

        });

    //Requesten som behandlar när en spelare vill hosta en lobby
    svr.Post("/HostLobby", [&](const httplib::Request& req, Response& res) {
  
            std::string playerId = req.get_header_value("Cookie");
            json message;
            std::string string = req.body;
            json requestMessage = json::parse(string);
            int amountOfPacks = 0;
            int cardPerMainDeckPack = 0; 
            int cardPerExtraDeckPack = 0; 
            bool parsingSuccessful = false;
            try
            {
                amountOfPacks = std::stoi((std::string)requestMessage["amountOfPacks"]);
                cardPerMainDeckPack = std::stoi((std::string)requestMessage["mainDeckCardsPerPack"]);
                cardPerExtraDeckPack = std::stoi((std::string)requestMessage["extraDeckCardsPerPack"]);
                parsingSuccessful = true;
            }
            catch (std::exception e) {
                std::cout << e.what();
                parsingSuccessful = false;
            }


            bool shouldHostLobby = false;
            {
                std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);
                auto iterator = playerToLobby.find(playerId);
                if (iterator == playerToLobby.end())
                {
                    shouldHostLobby = true;
                }
                else
                {
                    message["Accepted"] = true;
                    message["RefLink"] = "/DraftLobby?LobbyId=" + iterator->second;
                    res.set_content(message.dump(), "text/plain");
                    return;
                }

            }
            if (shouldHostLobby && parsingSuccessful)
            {   
                message["Accepted"] = true; 
                message["RefLink"] = "/DraftLobby?LobbyId=" + HostLobby(playerId, cardPerMainDeckPack, cardPerExtraDeckPack, amountOfPacks);
                res.set_content(message.dump(), "text/plain");
            }
            else
            {
                message["Accepted"] = false;

                res.set_content(message.dump(), "text/plain");
            }
            
            currentRequestsBeforeDestroy += 1;
            if (currentRequestsBeforeDestroy >= maxRequestsBeforeLobbyDestroy)
            {   
                RemoveInactiveLobbies();
                currentRequestsBeforeDestroy = 0;
            }

            //std::cout << "host grejen " << playerId << std::endl;
        });
    //Requesten som behandlar när en spelare vill starta lobbyn. Går bara om man är spelaren som hostade lobbyn
    svr.Post("/StartLobby", [&](const httplib::Request& req, Response& res) {

        std::string playerId = req.get_header_value("Cookie");
        std::lock_guard<std::mutex> lock = std::lock_guard(serverMutex);

   //     json messageJson = json::parse(req.body);

    //    std::string hej = messageJson.dump();
        
        auto iterator = activeLobbies.find(playerToLobby[playerId]);
        if (iterator != activeLobbies.end())
        {
            if (playerId._Equal(iterator->second.GetHost()) && !iterator->second.HasLobbyStarted())   
            {
                iterator->second.StartLobby(availableMainDeckCards,availableExtraCards);
                res.set_content("lobby started", "text/plain");
            }
            res.set_content("du ar inte host", "text/plain");
        }
        else
        {
            res.set_content("lobby finns inte", "text/plain");
        }
       // std::cout << "start grejen " << playerId << std::endl;
        });
    //Requesten som behandlar när en spelare vill gå med en lobby. Man kan bara vara med i en lobby samtidigt

    svr.Post("/JoinLobby", [&](const httplib::Request& req, Response& res) {    
            JoinLobby(req, res);
    //    std::cout << "join grejen " <<playerId  << std::endl;
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

        //std::cout << "pick grejen " << playerId << std::endl;

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
    std::string mainDeckEntry = pointOfEntry;
    std::string extraDeckEntry = pointOfEntry;
    std::filesystem::path mainDeckImagesPath = std::filesystem::path(mainDeckEntry.append("\\CardImages\\MainDeck"));
    std::filesystem::path extraDeckImagesPath = std::filesystem::path(extraDeckEntry.append("\\CardImages\\ExtraDeck"));

    for (auto const& card : std::filesystem::directory_iterator{mainDeckImagesPath})
    {
        availableMainDeckCards.push_back( "MainDeck/"+ card.path().filename().string());

       // std::cout << card.path().filename().string();
    }
    for (auto const& card : std::filesystem::directory_iterator{ extraDeckImagesPath })
    {
        availableExtraCards.push_back("ExtraDeck/" + card.path().filename().string());

        // std::cout << card.path().filename().string();
    }
}


//metoden för när man hosta en lobby
std::string DraftServer::HostLobby(const std::string& playerId, int mainDeckCardsPerPack, int extraDeckCardsPerPack, int amountOfPacks)
{   

    std::lock_guard<std::mutex> lockGuard(serverMutex);
    std::string uniqueLobbyId = std::move(GetUniqueLobbyURL());
    activeLobbies[uniqueLobbyId] = Lobby(playerId, mainDeckCardsPerPack, amountOfPacks, true, extraDeckCardsPerPack);
//    activeLobbies[std::to_string(lobbyId)].StartLobby(availableMainDeckCards);
    playerToLobby[playerId] = uniqueLobbyId;

    return uniqueLobbyId;
}

std::string DraftServer::GetUniqueLobbyURL()
{
    UUID globalGUID;
    if (UuidCreate(&globalGUID) == RPC_S_OK)
    {
        RPC_CSTR rpcGUID;
        RPC_STATUS status = UuidToString(&globalGUID, &rpcGUID);

        if (status == RPC_S_OK)
        {
            std::string stringGUID((char*)rpcGUID);
            RpcStringFreeA(&rpcGUID);
            return stringGUID;
          //  res.set_header("Set-Cookie", "PlayerId = " + stringGUID + "; Expires=Thu, 25 Jun 2024 07:28:00 GMT;");
        }
    }
    return "det sket sig";
}
void DraftServer::RemoveInactiveLobbies()
{
    std::vector<std::string> lobbysToRemove;
    {
        std::lock_guard<std::mutex> lockguard(serverMutex);
        auto iterator = activeLobbies.begin();


        for (; iterator != activeLobbies.end(); iterator++)
        {
            if (std::chrono::minutes(minutesBeforeDestruction) < std::chrono::system_clock::now() - iterator->second.GetTimeStamp())
            {
                lobbysToRemove.push_back(iterator->first);
            }
        }
    }
    
    for (int i = 0; i < lobbysToRemove.size(); i++)
    {
        RemoveLobby(lobbysToRemove[i]);
    }

}
//ofärdig metod för när man ska ta bort en lobby
void DraftServer::RemoveLobby(const std::string& lobbyId)
{   
    std::lock_guard<std::mutex> lockGuard(serverMutex);

    auto activeLobbiesIterator = activeLobbies.find(lobbyId);
    if(activeLobbiesIterator != activeLobbies.end())
    {
        activeLobbies.erase( activeLobbiesIterator);
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

void DraftServer::JoinLobby(const Request& req, Response& res)
{
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
}

void DraftServer::GiveCookie(const Request& req, Response& res)
{

    if (req.get_header_value("Cookie") == "")
    {
        UUID globalGUID;
        if (UuidCreate(&globalGUID) == RPC_S_OK)
        {
            RPC_CSTR rpcGUID;
            RPC_STATUS status = UuidToString(&globalGUID, &rpcGUID);

            if (status == RPC_S_OK)
            {
                std::string stringGUID((char*)rpcGUID);
                RpcStringFreeA(&rpcGUID);
                res.set_header("Set-Cookie", "PlayerId = " + stringGUID + "; Expires=Thu, 25 Okt 2024 07:28:00 GMT;SameSite=None");
            }
        }
    }
}
void DraftServer::ServeHTML(const Request& req, Response& res, const std::string& htmlPath )
{
    std::string tempString = pointOfEntry;
    std::ifstream ifStream;
    ifStream = std::ifstream(tempString.append("\\" + htmlPath));
    ifStream.seekg(0, ifStream.end);
    std::string html = std::string(ifStream.tellg(), 0);
    ifStream.seekg(0, ifStream.beg);
    ifStream.read(html.data(), html.size());
    res.set_content(html, "text/html");
}