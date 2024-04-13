#include "Lobby.h"
#include <algorithm>
#include <random>
//Lobby är objektet för de lobbies av spelare som skapas

//När en spelare vill välja att ta ett kort. Man kan inte ta ett nytt kort förrän alla andra har tagit 
void Lobby::PickCard(const std::string &playerId, int index)
{	
	UpdateTimeStamp();
	bool rotatePack = false;
	{
		std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

		if (index < 0 || index >= playerPacks[playerId].size() || playerPacks[playerId].size() == 0)
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
//När alla spelare har tagit 1 kort så startas en ny omgång där packen roterar.
void Lobby::RotatePacks(bool allPlayersHavePicked)
{
	UpdateTimeStamp();

	{
		std::lock_guard<std::mutex> lockGuard(*lobbyMutex);

		std::vector<std::string> firstElement = playerPacks.begin()->second;
		for (std::map<std::string, std::vector<std::string>>::iterator iterator = playerPacks.begin(); iterator != playerPacks.end(); ++iterator)
		{
			auto nextElement = std::next(iterator);
			if (nextElement == playerPacks.end())
			{
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

//oanvänd metod
const bool Lobby::IsDraftFinished()
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	return draftFinished;
}

//När en ny spelare vill joina lobbyn 
void Lobby::AddConnectedPlayer(const std::string &playerId)
{
	UpdateTimeStamp();

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	if (std::find(connectedPlayers.begin(), connectedPlayers.end(), playerId) == connectedPlayers.end())
	{
		connectedPlayers.push_back(playerId);
	}
}

//När man vill få de kortet man har tillgänglighet för att drafta 
const std::vector<std::string> Lobby::GetDraftableCardsPlayer(const std::string& playerId)
{
	UpdateTimeStamp();

	std::vector<std::string> stringToReturn;
	if (playerHavePicked[playerId])
	{
		return stringToReturn;
	}
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	for (int i = 0; i < playerPacks[playerId].size(); i++)
	{
		stringToReturn.push_back(playerPacks[playerId][i]);
	}

	return stringToReturn;
}


//När man vill få kortet man redan har tagit
const std::string Lobby::GetPickedCardsPlayer(const std::string& playerId)
{
	UpdateTimeStamp();

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

//frågar om en spelare redan är med i lobbyn. 
const bool Lobby::IsPlayerConnected(const std::string& playerId)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	if (std::find(connectedPlayers.begin(), connectedPlayers.end(), playerId) != connectedPlayers.end())
	{
		return true;
	}

	return false;
}

//Skapar packsen. Är utifrån en finit lista som är slumpad
void Lobby::CreatePacks()
{	

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);


	if (packsCreated == amountOfPacks)
	{
		draftFinished = true;
		return;
	}

	std::vector<std::string>* listToUse; 
	int amountToUse = 0; 
	if (useExtraDeck)
	{
		if (shouldCreateExtraDeckPack)
		{
			listToUse = &shuffledExtraDeckCards;
			amountToUse = extraDeckCardsPerPack;
			shouldCreateExtraDeckPack = false;
		}
		else
		{
			listToUse = &shuffledMainDeckCards;
			amountToUse = mainDeckCardsPerPack;
			shouldCreateExtraDeckPack = true;
		}
	}
	else
	{
		listToUse = &shuffledMainDeckCards;
		amountToUse = mainDeckCardsPerPack;
	}
	for (int i = 0; i < connectedPlayers.size(); i++)
	{	
		


		//playerPacks[connectedPlayers[i]];

		for (int z = 0; z < amountToUse; z++)
		{
			playerPacks[connectedPlayers[i]].push_back(listToUse->back());
			listToUse->pop_back();
		}

	}
	packsCreated += 1;
}

//Frågar om lobbyn har startat
const bool Lobby::HasLobbyStarted()
{
	UpdateTimeStamp();

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	
	return lobbyStarted;
}

//Ser vem som har hostat lobbyn
const std::string& Lobby::GetHost()
{


	return host;
}
//får en lista av alla connectade spelare
const std::vector<std::string> Lobby::GetConnectedPlayers()
{
	UpdateTimeStamp();

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	std::vector<std::string> messageToReturn;

	for (int i = 0; i < connectedPlayers.size(); i++)
	{
		messageToReturn.push_back(connectedPlayers[i]);
	}

	return messageToReturn;
}

void Lobby::UpdatePlayerSeenLobbyEnded(const std::string& playerId)
{
	std::lock_guard<std::mutex> lockGuard(*lobbyMutex);

	playerReciviedLobbyEnded[playerId] = true;

	if (playerReciviedLobbyEnded.size() >= connectedPlayers.size())
	{
		lobbyHasEnded = true;
	}
	//
	//}
}

//Startar en lobby. När en lobby är startad kan man inte längre gå med i den. 
void Lobby::StartLobby(const std::vector<std::string> &availableCards, const std::vector<std::string> &extraDeckCards) 
{
	UpdateTimeStamp();

	{
		std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

		lobbyStarted = true;

		shuffledMainDeckCards = availableCards;

		auto rng = std::default_random_engine(std::_Random_device());
		std::shuffle(shuffledMainDeckCards.begin(), shuffledMainDeckCards.end(), rng);

		if (shuffledMainDeckCards.size() < connectedPlayers.size() * mainDeckCardsPerPack * amountOfPacks)
		{
			int extraCards =  (connectedPlayers.size() * mainDeckCardsPerPack * amountOfPacks) - shuffledMainDeckCards.size() ;

			for (int i = 0; i < extraCards; i++)
			{
				shuffledMainDeckCards.push_back(shuffledMainDeckCards[0]);

				
			}
		}
		shuffledExtraDeckCards = extraDeckCards;

		std::shuffle(shuffledExtraDeckCards.begin(), shuffledExtraDeckCards.end(), rng);

		if (shuffledExtraDeckCards.size() < connectedPlayers.size() * extraDeckCardsPerPack * amountOfPacks)
		{
			int extraCards = (connectedPlayers.size() * extraDeckCardsPerPack * amountOfPacks) - shuffledExtraDeckCards.size();

			for (int i = 0; i < extraCards; i++)
			{
				shuffledExtraDeckCards.push_back(shuffledExtraDeckCards[0]);


			}
		}
		if (useExtraDeck)
		{
			amountOfPacks *= 2; 
		}
	}


	CreatePacks();
}
