#pragma once
#include "Card.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <mutex>


//alla dessa beskrivs i cpp filen 
class Lobby
{

private: 
	std::string lobbyId;

	std::vector<std::string> connectedPlayers;

	std::map<std::string, std::vector<std::string>> playerPacks;

	std::unordered_map<std::string, bool> playerHavePicked;

	std::unordered_map<std::string, bool> playerReciviedLobbyEnded;
	std::unordered_map<std::string, std::vector<std::string>> pickedCards;

	int mainDeckCardsPerPack = 0;
	int extraDeckCardsPerPack = 0;
	int amountOfPacks = 0; 

	std::unique_ptr<std::mutex> lobbyMutex = std::make_unique<std::mutex>();
		
	std::vector<std::string> shuffledMainDeckCards;
	std::vector<std::string> shuffledExtraDeckCards;

	int packsCreated = 0; 

	bool draftFinished = false;

	std::string host; 

	bool useExtraDeck = true; 
	bool shouldCreateExtraDeckPack = false; 
	
	bool lobbyHasEnded = false;
	bool lobbyStarted = false;
	std::chrono::system_clock::time_point timeStampLastAction;

	void RotatePacks(bool allPlayersHavePicked);

	void CreatePacks();
	void UpdateTimeStamp() { std::lock_guard<std::mutex> lockGuard(*lobbyMutex);  timeStampLastAction = std::chrono::system_clock::now(); }

public: 

	Lobby() = default;


	void PickCard(const std::string &playerId, int index);


	void StartLobby(const  std::vector<std::string> &availableCards, const std::vector<std::string> &extraDeckcards);

	

	void AddConnectedPlayer(const std::string &playerId);

	const bool HasLobbyStarted();

	const std::vector<std::string> GetDraftableCardsPlayer(const std::string& playerId);
	const std::string GetPickedCardsPlayer(const std::string& playerId);
	const std::vector<std::string> GetConnectedPlayers();

	const bool IsPlayerConnected(const std::string& playerId);

	const std::string& GetHost();

	const bool IsDraftFinished();

	const bool HasLobbyEnded() { std::lock_guard<std::mutex> lockguard(*lobbyMutex); return lobbyHasEnded; }
	const std::chrono::system_clock::time_point& GetTimeStamp() { std::lock_guard<std::mutex> lockGuard(*lobbyMutex); return timeStampLastAction; }

	void UpdatePlayerSeenLobbyEnded(const std::string&	playerId);

	Lobby(const std::string &firstPlayer ,int mainDeckCardsPerPack, int amountOfPacks, bool useExtraDeck, int extraDeckCardsPerPack)
	{
		connectedPlayers.push_back(firstPlayer);
		host = firstPlayer;
		this->mainDeckCardsPerPack = mainDeckCardsPerPack;
		this->amountOfPacks = amountOfPacks;
		timeStampLastAction = std::chrono::system_clock::now();
		this->extraDeckCardsPerPack = extraDeckCardsPerPack;
		
	}
};