#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "GameState.h"
#include "Deck.h"
#include "Game.h"
#include "Lobby.h"
#include "Login.h"
#include "Network.h"

gameState g_gameState;

int main() 
{
	g_gameState = LOGIN_VIEW;

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
				//lobby = new Lobby();
				//lobby->addToPlayerList("Sarah", sf::Color::Cyan, true);
				//lobby->addToPlayerList("Josh", sf::Color::Blue, false);
				//lobby->addToPlayerList("Minh", sf::Color::Blue, false);
				//lobby->addToPlayerList("Bob", sf::Color::Blue, false, Lobby::FRONT);
				//lobby->addToPlayerList("Hoang", sf::Color::Blue, false, Lobby::FRONT);
				//Network::getInstance().connect();
				//Network::getInstance().startListening();
				//Network::getInstance().startProcessingMessageQueue();
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
				//playerList[0]->setTurn(true);
				//playerList[2]->setSheriffStatus(true);
				//game->addToUserHand(Card::APPLE);
				//game->addToUserHand(Card::APPLE);
				//game->addToUserHand(Card::CHICKEN);
				//game->addToUserHand(Card::APPLE);
				//game->addToUserHand(Card::CHICKEN);
				//game->addToUserHand(Card::CROSSBOW);
				//game->userHandUI();

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
