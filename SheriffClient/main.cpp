#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "Game.h"
#include "Lobby.h"
#include "Login.h"
#include "Network.h"

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

	Login login;
	network.addObserver(&login);

	while (login.running()) {
		// Update
		login.update();

		// Render
		login.render();

	}

	Lobby lobby;
	network.addObserver(&lobby);

	lobby.addToPlayerList("An", sf::Color::Red, true);
	lobby.addToPlayerList("Hoang", sf::Color::Blue, false);
	lobby.addToPlayerList("Tuan", sf::Color::Cyan, false);
	lobby.addToPlayerList("Minh", sf::Color::Magenta, false);
	lobby.addToPlayerList("Tu", sf::Color::White, false);
	lobby.addToPlayerList("Thanh", sf::Color::Yellow, false);

	//lobby.removeFromPlayerList("Tuan");

	while (lobby.running()) {
		lobby.update();

		lobby.render();
	}

	// End application
	return 0;
}
