#pragma once
#include "Card.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <mutex>
class Lobby
{

private: 
	std::string lobbyId;

	std::vector<std::string> connectedPlayers;

	std::map<std::string, std::vector<std::string>> packsInRotation;

	std::unordered_map<std::string, bool> playerHavePicked;

	std::unordered_map<std::string, std::vector<std::string>> pickedCards;

	int cardsPerPack = 0;
	int amountOfPacks = 0; 

	std::mutex lobbyMutex;
		
public: 

	Lobby() = default;

	void PickCard(const std::string &playerId, int index);

	void StartLobby();

	void StartNewRotation(bool allPlayersHavePicked);

	void AddConnectedPlayer(const std::string &playerId);

	std::string GetDraftableCardsPlayer(const std::string &playerId);

	Lobby( std::vector<std::string> connectedPlayers ,int cardsPerPack, int amountOfPacks)
	{
		this->connectedPlayers = connectedPlayers;
		this->cardsPerPack = cardsPerPack;
		this->amountOfPacks = amountOfPacks;

		
	}
};