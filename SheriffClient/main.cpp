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
#include "include/Network/Network.h"

gameState g_gameState;

int main() 
{
	g_gameState = GAME_VIEW;

	Login* login = nullptr;
	Lobby* lobby = nullptr;
	Game* game = nullptr;
	
	while (true) {
		switch (g_gameState)
		{
		case LOGIN_VIEW:
			if (!login) {
				delete game;
				Network::getInstance().removeObserver(game);

				login = new Login();
				Network::getInstance().addObserver(login);
			}
			login->update();
			login->render();

			break;
		
		case LOBBY_VIEW:
			if (!lobby) {
				std::string username = login->getUsername();
				delete login;
				Network::getInstance().removeObserver(login);
				lobby = new Lobby();
				Network::getInstance().addObserver(lobby);
				lobby->addToPlayerList(username, sf::Color::Cyan, true);

				// Send response message to notify server that lobby has initiated
				json receivedLoginMessage;
				receivedLoginMessage["MessageType"] = "GAME_ACCEPT_PLAYER";
				receivedLoginMessage["PlayerName"] = username;
				Network::getInstance().respondMessage(receivedLoginMessage);

				// For testing only
				//lobby->addToPlayerList("Hoang", sf::Color::Blue, false);
				//lobby->addToPlayerList("Tuan", sf::Color::Red, false);
				//lobby->addToPlayerList("Minh", sf::Color::Magenta, false);
				//lobby->addToPlayerList("Tu", sf::Color::White, false);
				//lobby->addToPlayerList("Thanh", sf::Color::Yellow, false, Lobby::FRONT);
			}
			lobby->update();
			lobby->render();

			break;

		case GAME_VIEW:
			if (!game) {
				// For testing only
				lobby = new Lobby();
				lobby->addToPlayerList("Sarah", sf::Color::Cyan, true);
				lobby->addToPlayerList("Josh", sf::Color::Blue, false);
				lobby->addToPlayerList("Minh", sf::Color::Blue, false);
				lobby->addToPlayerList("Bob", sf::Color::Blue, false, Lobby::FRONT);
				lobby->addToPlayerList("Hoang", sf::Color::Blue, false, Lobby::FRONT);
				Network::getInstance().connect();
				Network::getInstance().startListening();
				Network::getInstance().startProcessingMessageQueue();
				// EoT

				std::vector<Player*> playerList = lobby->getPlayerList();
				delete lobby;
				Network::getInstance().removeObserver(lobby);
				game = new Game(playerList);
				Network::getInstance().addObserver(game);
				// Send a response message to nofity game started
				json receivedStartMessage;
				receivedStartMessage["MessageType"] = "GAME_START";
				Network::getInstance().respondMessage(receivedStartMessage);

				// For testing only
				playerList[2]->setTurn(true);
				playerList[0]->setSheriffStatus(true);
				playerList[2]->increasePlayerGoodsAmount(Card::APPLE, 2);
				playerList[2]->increasePlayerGoodsAmount(Card::PEPPER, 3);
				playerList[2]->setPlayerMedalStatus(Card::PEPPER, Player::MedalStatus::BLACK_MARKET_PLUS);
				playerList[2]->setPlayerMedalStatus(Card::MEAD, Player::MedalStatus::BLACK_MARKET);
				playerList[2]->setPlayerMedalStatus(Card::SILK, Player::MedalStatus::BLACK_MARKET_PLUS);
				playerList[2]->increasePlayerGoodsAmount(Card::SILK, 2);
				playerList[0]->increasePlayerGoodsAmount(Card::APPLE, 1);
				playerList[0]->increasePlayerGoodsAmount(Card::MEAD, 1);
				playerList[1]->increasePlayerGoodsAmount(Card::CROSSBOW, 4);
				game->addToUserHand(Card::APPLE);
				game->addToUserHand(Card::APPLE);
				game->addToUserHand(Card::CHICKEN);
				game->addToUserHand(Card::APPLE);
				game->addToUserHand(Card::CHICKEN);
				game->addToUserHand(Card::CROSSBOW);
				game->userHandUI();
				// EoT

			}
			game->update();
			game->render();

			break;

		default:
			break;
		}
	}

	// End application
	return 0;
}
