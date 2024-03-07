#include "Lobby.h"
#include <algorithm>
#include <random>

void Lobby::PickCard(const std::string &playerId, int index)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	if (index < 0 || index > playerPacks.size())
	{
		return;
	}

	if (playerHavePicked[playerId] == false)
	{
		pickedCards[playerId].push_back(playerPacks[playerId][index]);
		
		playerPacks[playerId].erase(playerPacks[playerId].begin() + index);

		playerHavePicked[playerId] = true;


		bool allHavePicked = true;
		for(std::pair<std::string,bool> valuePair : playerHavePicked)
		{
			if (!valuePair.second)
			{
				allHavePicked = false;
			}
		}
		if (allHavePicked)
		{
			RotatePacks(allHavePicked);
		}
		

	}
}

void Lobby::RotatePacks(bool allPlayersHavePicked)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	std::vector<std::string> firstElement = playerPacks.begin()->second;
	for (std::map<std::string, std::vector<std::string>>::iterator iterator = playerPacks.begin(); iterator != playerPacks.end();)
	{		
		auto nextElement = ++iterator;
		if (nextElement == playerPacks.end())
		{
			iterator->second = firstElement;
			continue;
		}
		iterator->second = nextElement->second; 
	}
	if (playerPacks[0].size() == 0)
	{
		CreatePacks();
	}
}

void Lobby::AddConnectedPlayer(const std::string &playerId)
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	if (std::find(connectedPlayers.begin(), connectedPlayers.end(), playerId) == connectedPlayers.end())
	{
		connectedPlayers.push_back(playerId);
	}
}

std::string Lobby::GetDraftableCardsPlayer(const std::string& playerId)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	std::string stringToReturn = "";

	for (int i = 0; i < playerPacks[playerId].size(); i++)
	{
		stringToReturn += playerPacks[playerId][i];
		if (i != playerPacks[playerId].size() - 1)
		{
			stringToReturn += ":";
		}
	}

	return stringToReturn;
}

void Lobby::CreatePacks()
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);


	for (int i = 0; i < connectedPlayers.size(); i++)
	{
		playerPacks[connectedPlayers[i]];

		for (int z = 0; z < cardsPerPack; z++)
		{
			playerPacks[connectedPlayers[i]].push_back(shuffledCardList.back());
			shuffledCardList.pop_back();
		}
	}
}

void Lobby::StartLobby(const std::vector<std::string> &availableCards) 
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);


	shuffledCardList = availableCards;

	auto rng = std::default_random_engine{};
	std::shuffle(shuffledCardList.begin(), shuffledCardList.end(), rng);

	CreatePacks();
}