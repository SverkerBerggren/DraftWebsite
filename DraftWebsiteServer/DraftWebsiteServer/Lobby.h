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

	std::map<std::string, std::vector<std::string>> playerPacks;

	std::unordered_map<std::string, bool> playerHavePicked;

	std::unordered_map<std::string, std::vector<std::string>> pickedCards;

	int cardsPerPack = 0;
	int amountOfPacks = 0; 

	std::unique_ptr<std::mutex> lobbyMutex = std::make_unique<std::mutex>();
		
	std::vector<std::string> shuffledCardList;

public: 

	Lobby() = default;


	void PickCard(const std::string &playerId, int index);

	void CreatePacks();

	void StartLobby(const  std::vector<std::string> &availableCards);

	void RotatePacks(bool allPlayersHavePicked);

	void AddConnectedPlayer(const std::string &playerId);

	std::string GetDraftableCardsPlayer(const std::string &playerId);

	Lobby( std::string firstPlayer ,int cardsPerPack, int amountOfPacks)
	{
		connectedPlayers.push_back(firstPlayer);
		this->cardsPerPack = cardsPerPack;
		this->amountOfPacks = amountOfPacks;

		
	}
};