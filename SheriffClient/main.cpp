#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "include/Core/GameState.h"
#include "include/Entities/Deck.h"
#include "include/Core/Game.h"
#include "include/Core/Lobby.h"
#include "include/Core/Login.h"
#include "include/Core/VictoryScreen.h"
#include "include/Network/Network.h"
#include "include/Utils/StreamBuf.h"

#include <experimental/filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

// Helper function: Generate a timestamped filename
std::string generateLogFilename() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm tm;
#ifdef _WIN32
	localtime_s(&tm, &now_c);
#else
	localtime_r(&now_c, &tm);
#endif

	std::ostringstream oss;
	oss << "Logs/log_" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".txt";
	return oss.str();
}

gameState g_gameState;

int main() 
{
	// Create Logs directory if not exists
	std::experimental::filesystem::create_directory("Logs");

	// Generate log file name
	std::string logFileName = generateLogFilename();

	// Open log file
	std::ofstream logFile(logFileName);
	TeeBuf teeBuf(std::cout.rdbuf(), logFile.rdbuf());
	std::ostream out(&teeBuf);

	// Replace std::cout with the tee stream
	std::cout.rdbuf(&teeBuf);

	g_gameState = LOGIN_VIEW;

	Login* login = nullptr;
	Lobby* lobby = nullptr;
	Game* game = nullptr;
	Summary* summary = nullptr;
	
	while (true) {
		switch (g_gameState)
		{
		case LOGIN_VIEW:
			if (!login) {
				delete summary;
				summary = nullptr;

				login = new Login();
				Network::getInstance().addObserver(login);
			}
			login->update();
			login->render();

			break;
		
		case LOBBY_VIEW:
			if (!lobby) {
				std::string username = login->getUsername();

				Network::getInstance().removeObserver(login);
				delete login;
				login = nullptr;

				lobby = new Lobby();
				Network::getInstance().addObserver(lobby);
				lobby->addToPlayerList(username, sf::Color::Cyan, true);

				// Send response message to notify server that lobby has initiated
				json receivedLoginMessage;
				receivedLoginMessage["MessageType"] = "GAME_ACCEPT_PLAYER";
				receivedLoginMessage["PlayerName"] = username;
				Network::getInstance().respondMessage(receivedLoginMessage);
			}
			lobby->update();
			lobby->render();

			break;

		case GAME_VIEW:
			if (!game) {
				// For testing only
				//lobby = new Lobby();
				//lobby->addToPlayerList("Sarah", sf::Color::Cyan, true);
				//lobby->addToPlayerList("Josh", sf::Color::Blue, false);
				//lobby->addToPlayerList("Minh", sf::Color::Blue, false);
				//lobby->addToPlayerList("Bob", sf::Color::Blue, false, Lobby::FRONT);
				//lobby->addToPlayerList("Hoang", sf::Color::Blue, false, Lobby::FRONT);
				//lobby->addToPlayerList("TuanThanhGay", sf::Color::Blue, false, Lobby::FRONT);
				//Network::getInstance().connect();
				//Network::getInstance().startListening();
				//Network::getInstance().startProcessingMessageQueue();
				// EoT

				std::vector<Player*> playerList = lobby->getPlayerList();

				Network::getInstance().removeObserver(lobby);
				delete lobby;
				lobby = nullptr;

				game = new Game(playerList);
				Network::getInstance().addObserver(game);
				// Send a response message to nofity game started
				json receivedStartMessage;
				receivedStartMessage["MessageType"] = "GAME_START";
				Network::getInstance().respondMessage(receivedStartMessage);

				// For testing only
				//playerList[2]->setTurn(true);
				//playerList[0]->setSheriffStatus(true);
				//playerList[0]->increasePlayerGoodsAmount(Card::APPLE, 10);
				//playerList[0]->increasePlayerGoodsAmount(Card::CHEESE, 7);
				//playerList[0]->increasePlayerGoodsAmount(Card::BREAD, 6);
				//playerList[0]->increasePlayerGoodsAmount(Card::CHICKEN, 8);
				//playerList[2]->increasePlayerGoodsAmount(Card::BREAD, 4);
				//playerList[2]->increasePlayerGoodsAmount(Card::PEPPER, 3);
				//playerList[0]->setPlayerMedalStatus(Card::PEPPER, Player::MedalStatus::BLACK_MARKET_BOTH);
				//playerList[0]->setPlayerMedalStatus(Card::MEAD, Player::MedalStatus::BLACK_MARKET_BOTH);
				//playerList[0]->setPlayerMedalStatus(Card::SILK, Player::MedalStatus::BLACK_MARKET_BOTH);
				//playerList[2]->increasePlayerGoodsAmount(Card::SILK, 2);
				//playerList[0]->increasePlayerGoodsAmount(Card::MEAD, 1);
				//playerList[1]->increasePlayerGoodsAmount(Card::CROSSBOW, 4);
				//game->addToUserHand(Card::APPLE);
				//game->addToUserHand(Card::APPLE);
				//game->addToUserHand(Card::CHICKEN);
				//game->addToUserHand(Card::APPLE);
				//game->addToUserHand(Card::CHICKEN);
				//game->addToUserHand(Card::CROSSBOW);
				//game->userHandUI();
				// EoT

			}
			game->update();
			game->render();

			break;

		case END_VIEW:
			if (!summary) {
				std::vector<Player*> playerList = game->getPlayerList();

				Network::getInstance().removeObserver(game);
				delete game;
				game = nullptr;

				summary = new Summary(playerList);
			}

			summary->update();
			summary->render();

			break;

		case INVALID:
			// End process
			Network::getInstance().disconnect();
			return 0;

		default:
			break;
		}
	}

	// End application
	return 0;
}
