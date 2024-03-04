#include "Lobby.h"


void Lobby::PickCard(std::string playerId, int index)
{	

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
	std::vector<Card> firstElement = packsInRotation.begin()->second;
	for (std::map<std::string, std::vector<Card>>::iterator iterator = packsInRotation.begin(); iterator != packsInRotation.end();)
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

void Lobby::InitializeLobby(std::vector<std::string> playersInLobby )
{

}