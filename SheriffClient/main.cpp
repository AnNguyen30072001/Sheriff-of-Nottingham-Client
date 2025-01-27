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
	//if (!Network::getInstance().connect()) {
	//	// TO DO: Show Popup saying connection failed
	//	std::cerr << "Failed to connect to the server." << std::endl;
	//	return -1;
	//}
	//Network::getInstance().startListening();
	//Network::getInstance().startProcessingMessageQueue();

	//std::cout << "Connected to the server." << std::endl;
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
				std::vector<Player*> playerList = lobby->getPlayerList();
				playerList[0]->setTurn(true);
				delete lobby;
				Network::getInstance().removeObserver(lobby);
				game = new Game(playerList);
				Network::getInstance().addObserver(game);

				// For testing only
				game->addToUserHand(Card::APPLE);
				game->addToUserHand(Card::CROSSBOW);
				game->addToUserHand(Card::CHICKEN);
				game->addToUserHand(Card::BREAD);
				game->addToUserHand(Card::APPLE);
				game->addToUserHand(Card::APPLE);
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
