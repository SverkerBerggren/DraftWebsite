#include "Lobby.h"


void Lobby::PickCard(const std::string &playerId, int index)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(lobbyMutex);

	if (index < 0 || index > packsInRotation.size())
	{
		return;
	}

	if (playerHavePicked[playerId] == false)
	{
		pickedCards[playerId].push_back(packsInRotation[playerId][index]);
		
		packsInRotation[playerId].erase(packsInRotation[playerId].begin() + index);

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
			StartNewRotation(allHavePicked);
		}
		

	}
}

void Lobby::StartNewRotation(bool allPlayersHavePicked)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(lobbyMutex);

	std::vector<std::string> firstElement = packsInRotation.begin()->second;
	for (std::map<std::string, std::vector<std::string>>::iterator iterator = packsInRotation.begin(); iterator != packsInRotation.end();)
	{		
		auto nextElement = ++iterator;
		if (nextElement == packsInRotation.end())
		{
			iterator->second = firstElement;
			continue;
		}
		iterator->second = nextElement->second; 
	}

}

void Lobby::AddConnectedPlayer(const std::string &playerId)
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(lobbyMutex);

	if (std::find(connectedPlayers.begin(), connectedPlayers.end(), playerId) == connectedPlayers.end())
	{
		connectedPlayers.push_back(playerId);
	}
}

std::string Lobby::GetDraftableCardsPlayer(const std::string& playerId)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(lobbyMutex);
	std::string stringToReturn = "";

	for (int i = 0; i < packsInRotation[playerId].size(); i++)
	{
		stringToReturn += packsInRotation[playerId][i];
		if (i != packsInRotation[playerId].size() - 1)
		{
			stringToReturn += ":";
		}
	}
}

void Lobby::StartLobby()
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(lobbyMutex);

}