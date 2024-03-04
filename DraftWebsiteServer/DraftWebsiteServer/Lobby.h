#pragma once
#include "Card.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

class Lobby
{

private: 
	std::string lobbyId;

	std::vector<std::string> connectedPlayers;

	std::map<std::string, std::vector<Card>> packsInRotation;

	std::unordered_map<std::string, bool> playerHavePicked;

	std::unordered_map<std::string, std::vector<Card>> pickedCards;

	int cardsPerPack;
	int amountOfPacks; 

	void InitializeLobby(const std::vector <std::string> &playersInLobby );
	
	
public: 




	void PickCard(const std::string &playerId, int index);

	std::string GetDraftableCards(const std::string &playerId);

	void StartNewRotation(bool allPlayersHavePicked);

	

	Lobby(const std::vector<std::string> &connectedPlayers ,int cardsPerPack, int amountOfPacks)
	{
		this->connectedPlayers = connectedPlayers;
		this->cardsPerPack = cardsPerPack;
		this->amountOfPacks = amountOfPacks;
		InitializeLobby(connectedPlayers);
	}
};