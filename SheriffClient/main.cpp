#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "GameState.h"
#include "Game.h"
#include "Lobby.h"
#include "Login.h"
#include "Network.h"

gameState g_gameState;

int main() 
{
	Network& network = Network::getInstance("127.0.0.1", 8080);

	if (!network.connect()) {
		std::cerr << "Failed to connect to the server." << std::endl;
		return -1;
	}
	network.startListening();
	network.startProcessingMessageQueue();

	std::cout << "Connected to the server." << std::endl;
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
				network.removeObserver(game);

				login = new Login();
				network.addObserver(login);
			}
			login->update();
			login->render();

			break;
		
		case LOBBY_VIEW:
			if (!lobby) {
				std::string username = login->getUsername();
				delete login;
				network.removeObserver(login);
				lobby = new Lobby();
				network.addObserver(lobby);
				lobby->addToPlayerList(username, sf::Color::Red, true);
				lobby->addToPlayerList("Hoang", sf::Color::Blue, false);
				lobby->addToPlayerList("Tuan", sf::Color::Cyan, false);
				lobby->addToPlayerList("Minh", sf::Color::Magenta, false);
				lobby->addToPlayerList("Tu", sf::Color::White, false);
				lobby->addToPlayerList("Thanh", sf::Color::Yellow, false);
			}
			lobby->update();
			lobby->render();

			break;

		case GAME_VIEW:
			if (!game) {
				std::vector<Player*> playerList = lobby->getPlayerList();
				delete lobby;
				network.removeObserver(lobby);
				game = new Game(playerList);
				network.addObserver(game);
			}
			game->update();
			game->render();

			break;

		default:
			break;
		}
	}

	//while (login->running()) {
	//	// Update
	//	login->update();

	//	// Render
	//	login->render();

	//}

	//Lobby* lobby = new Lobby();
	//network.addObserver(lobby);

	//lobby->addToPlayerList("An", sf::Color::Red, true);
	//lobby->addToPlayerList("Hoang", sf::Color::Blue, false);
	//lobby->addToPlayerList("Tuan", sf::Color::Cyan, false);
	//lobby->addToPlayerList("Minh", sf::Color::Magenta, false);
	//lobby->addToPlayerList("Tu", sf::Color::White, false);
	//lobby->addToPlayerList("Thanh", sf::Color::Yellow, false);

	////lobby.removeFromPlayerList("Tuan");

	//while (lobby->running()) {
	//	lobby->update();

	//	lobby->render();
	//}

	// End application
	return 0;
}
