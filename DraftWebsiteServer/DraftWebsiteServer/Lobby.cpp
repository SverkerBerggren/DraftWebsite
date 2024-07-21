#include "Lobby.h"
#include <assert.h>
#include <algorithm>
#include <random>
//Lobby �r objektet f�r de lobbies av spelare som skapas

//N�r en spelare vill v�lja att ta ett kort. Man kan inte ta ett nytt kort f�rr�n alla andra har tagit 
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
//N�r alla spelare har tagit 1 kort s� startas en ny omg�ng d�r packen roterar.
void Lobby::RotatePacks(bool allPlayersHavePicked)
{
	UpdateTimeStamp();
	bool shouldCreatePacks = false;
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

		shouldCreatePacks = playerPacks[playerPacks.begin()->first].size() == 0;
		int PlayerPackSize = -1;
		int CardSum = 0;
		for (auto const& PlayerPack : playerPacks)
		{
			if (PlayerPackSize == -1) {
				PlayerPackSize = PlayerPack.second.size();
			}
			else {
				assert(PlayerPackSize == PlayerPack.second.size());
			}
			CardSum += PlayerPack.second.size();
		}
		int pickCardSize = -1;
		for (auto const& Pack : pickedCards)
		{
			if (pickCardSize == -1) { pickCardSize == Pack.second.size(); }
			else 
			{
				assert(pickCardSize == Pack.second.size()); 
			}
			CardSum += Pack.second.size();
		}
		assert(CardSum == (packsCreated * 15 * playerPacks.size()));
	}
	if (shouldCreatePacks)
	{
		CreatePacks();
	}
	
}

//oanv�nd metod
const bool Lobby::IsDraftFinished()
{
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	return draftFinished;
}

//N�r en ny spelare vill joina lobbyn 
void Lobby::AddConnectedPlayer(const std::string &playerId)
{
	UpdateTimeStamp();

	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);

	if (std::find(connectedPlayers.begin(), connectedPlayers.end(), playerId) == connectedPlayers.end())
	{
		connectedPlayers.push_back(playerId);
	}
}

//N�r man vill f� de kortet man har tillg�nglighet f�r att drafta 
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


//N�r man vill f� kortet man redan har tagit
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

//fr�gar om en spelare redan �r med i lobbyn. 
const bool Lobby::IsPlayerConnected(const std::string& playerId)
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	if (std::find(connectedPlayers.begin(), connectedPlayers.end(), playerId) != connectedPlayers.end())
	{
		return true;
	}

	return false;
}

//Skapar packsen. �r utifr�n en finit lista som �r slumpad
void Lobby::CreatePacks()
{	
	std::lock_guard<std::mutex> lockGuard = std::lock_guard<std::mutex>(*lobbyMutex);
	if (packsCreated == amountOfPacks)
	{
		draftFinished = true;
		return;
	}
	if (!m_PackCreationFunc)
	{
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
	}
	else {
		for (int i = 0; i < connectedPlayers.size(); i++) {
			playerPacks[connectedPlayers[i]] = m_PackCreationFunc();
		}
	}
	packsCreated += 1;
}

//Fr�gar om lobbyn har startat
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
//f�r en lista av alla connectade spelare
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
void Lobby::LoggDraftToSQL(sqlite3* database)
{
	
	std::lock_guard<std::mutex> lockGuard(*lobbyMutex);



	if (hasLoggedFinishedDraft)
	{
		return;
	}
	hasLoggedFinishedDraft = true;

	for (std::string playerId : connectedPlayers)
	{	
		int playerDraftcount = 0; 
		std::string playerDraftCountQuery = "SELECT MAX(draftCount) FROM PlayerDrafts WHERE playerID = ?1;";
		sqlite3_stmt* playerDraftCountStatement = nullptr;
		
		const char* tailPointer = nullptr;
		sqlite3_prepare_v2(database,playerDraftCountQuery.data(), playerDraftCountQuery.size(), &playerDraftCountStatement, &tailPointer);
		sqlite3_bind_text(playerDraftCountStatement, 1, playerId.data(), playerId.size(), SQLITE_TRANSIENT);
		
		if (sqlite3_step(playerDraftCountStatement) == SQLITE_ROW)
		{
			playerDraftcount = sqlite3_column_int(playerDraftCountStatement, 0) +1;
			char* pointer = (char*)sqlite3_column_text(playerDraftCountStatement, 1);
			if (pointer != nullptr)
			{
				std::string debug(pointer);
			}
		}
		sqlite3_reset(playerDraftCountStatement);
		sqlite3_finalize(playerDraftCountStatement);
		LoggPlayerDraft(playerId, playerDraftcount, database);
	}

}

void Lobby::LoggPlayerDraft(const std::string& playerId, int draftCount, sqlite3* database)
{	
	//std::string date = std::format("{0:%F %R %Z}", timeStampLastAction);
	std::string date = "placeholder";
	
	std::string playerDraftInsert = "INSERT INTO PlayerDrafts VALUES( ?1,?2,?3);";
	
	sqlite3_stmt* playerDraftstatement = nullptr;
	const char* tailPointer = nullptr;
	sqlite3_prepare_v2(database, playerDraftInsert.data(), playerDraftInsert.size(), &playerDraftstatement, &tailPointer);

	sqlite3_bind_int(playerDraftstatement,3,draftCount);
	sqlite3_bind_text(playerDraftstatement, 1, date.data(), date.size(), SQLITE_TRANSIENT);
	sqlite3_bind_text(playerDraftstatement, 2, playerId.data(), playerId.size(), SQLITE_TRANSIENT);

	sqlite3_step(playerDraftstatement);

	sqlite3_reset(playerDraftstatement);
	sqlite3_finalize(playerDraftstatement);


	//bygger p� att det bara finns 1 kopia av varje kort 
	for (std::string cardDrafted : pickedCards[playerId])
	{
		std::string cardsInDraftInsert = "INSERT INTO CardsInDraft VALUES( ?1,?2,?3,?4);";

		sqlite3_stmt* cardsInDraftStatement = nullptr;
		const char* tailPointer = nullptr;
		sqlite3_prepare_v2(database, cardsInDraftInsert.data(), cardsInDraftInsert.size(), &cardsInDraftStatement, &tailPointer);

		sqlite3_bind_text(cardsInDraftStatement, 1, playerId.data(), playerId.size(), SQLITE_TRANSIENT);
		sqlite3_bind_int(cardsInDraftStatement, 2, draftCount);
		sqlite3_bind_text(cardsInDraftStatement, 3, cardDrafted.data(), cardDrafted.size(), SQLITE_TRANSIENT);
		sqlite3_bind_int(cardsInDraftStatement, 4, 1);

		int message = sqlite3_step(cardsInDraftStatement);

		

		sqlite3_reset(cardsInDraftStatement);
		sqlite3_finalize(cardsInDraftStatement);

	}

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

//Startar en lobby. N�r en lobby �r startad kan man inte l�ngre g� med i den. 
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
				if (shuffledExtraDeckCards.size() != 0)
				{
					shuffledMainDeckCards.push_back(shuffledMainDeckCards[0]);
				}
				else {
					shuffledMainDeckCards.push_back("");
				}
			}
		}
		shuffledExtraDeckCards = extraDeckCards;

		std::shuffle(shuffledExtraDeckCards.begin(), shuffledExtraDeckCards.end(), rng);

		if (shuffledExtraDeckCards.size() < connectedPlayers.size() * extraDeckCardsPerPack * amountOfPacks)
		{
			int extraCards = (connectedPlayers.size() * extraDeckCardsPerPack * amountOfPacks) - shuffledExtraDeckCards.size();

			for (int i = 0; i < extraCards; i++)
			{
				if(shuffledExtraDeckCards.size() != 0)
				{
					shuffledExtraDeckCards.push_back(shuffledExtraDeckCards[0]);
				}
				else {
					shuffledExtraDeckCards.push_back("");
				}
			}
		}
		if (useExtraDeck)
		{
			amountOfPacks *= 2; 
		}
	}


	CreatePacks();
}
