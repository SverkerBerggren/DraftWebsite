#include "Lobby.h"
#include <algorithm>
#include <random>

void Lobby::PickCard(const std::string &playerId, int index)
{	

	bool rotatePack = false;
	{
		std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

		if (index < 0 || index > playerPacks[playerId].size() || playerPacks[playerId].size() == 0)
		{
			return;
		}

		if (playerHavePicked[playerId] == false)
		{
			pickedCards[playerId].push_back(playerPacks[playerId][index]);

			playerPacks[playerId].erase(playerPacks[playerId].begin() + index);

			playerHavePicked[playerId] = true;


			bool allHavePicked = true;
			for (std::pair<std::string, bool> valuePair : playerHavePicked)
			{
				if (!valuePair.second)
				{
					allHavePicked = false;
				}
			}
			if (allHavePicked)
			{
				rotatePack = true;
			}
		}
	}

	if (rotatePack)
	{
		RotatePacks(rotatePack);
	}
}

void Lobby::RotatePacks(bool allPlayersHavePicked)
{	
	{
		std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

		std::vector<std::string> firstElement = playerPacks.begin()->second;
		for (std::map<std::string, std::vector<std::string>>::iterator iterator = playerPacks.begin(); iterator != playerPacks.end();)
		{
			auto nextElement = ++iterator;
			if (nextElement == playerPacks.end())
			{
				--iterator;
				iterator->second = firstElement;
				break;
			}
			iterator->second = nextElement->second;
		}

		for (auto playerPicked : playerHavePicked)
		{
			playerHavePicked[playerPicked.first] = false;
		}

	}
	if (playerPacks[playerPacks.begin()->first].size() == 0 )
	{
		CreatePacks();
	}
}

bool Lobby::IsDraftFinished()
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	return draftFinished;
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



std::string Lobby::GetPickedCardsPlayer(const std::string& playerId)
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	std::string stringToReturn = "";

	for (int i = 0; i < pickedCards[playerId].size(); i++)
	{
		stringToReturn += pickedCards[playerId][i];
		if (i != pickedCards[playerId].size() - 1)
		{
			stringToReturn += ":";
		}
	}

	return stringToReturn;
}


void Lobby::CreatePacks()
{	

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	if (packsCreated == amountOfPacks)
	{
		draftFinished = true;
		return;
	}

	for (int i = 0; i < connectedPlayers.size(); i++)
	{
		playerPacks[connectedPlayers[i]];

		for (int z = 0; z < cardsPerPack; z++)
		{
			playerPacks[connectedPlayers[i]].push_back(shuffledCardList.back());
			shuffledCardList.pop_back();
		}

	}
	packsCreated += 1;
}

bool Lobby::HasLobbyStarted()
{

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	
	return lobbyStarted;
}

bool Lobby::HasLobbyStarted()
{

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	return lobbyStarted;
}
void Lobby::StartLobby(const std::vector<std::string> &availableCards) 
{

	{
		std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

		lobbyStarted = true;

		shuffledCardList = availableCards;

		auto rng = std::default_random_engine{};
		std::shuffle(shuffledCardList.begin(), shuffledCardList.end(), rng);

		if (shuffledCardList.size() < connectedPlayers.size() * cardsPerPack * amountOfPacks)
		{
			int extraCards =  (connectedPlayers.size() * cardsPerPack * amountOfPacks) - shuffledCardList.size() ;

			for (int i = 0; i < extraCards; i++)
			{
				shuffledCardList.push_back(shuffledCardList[0]);

				
			}
		}
	}


	CreatePacks();
}