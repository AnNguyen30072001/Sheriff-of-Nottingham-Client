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
	Network network("127.0.0.1", 8080);

	if (!network.connect()) {
		std::cerr << "Failed to connect to the server." << std::endl;
		return -1;
	}

	std::cout << "Connected to the server." << std::endl;
	network.startListening();

	Login login;

	while (login.running()) {
		// Update
		login.update();

		// Periodically check for messages
		std::string message;
		while (network.receiveMessage(message)) {
			std::cout << "Server: " << message << std::endl;
			// Process the message (update UI, game state, etc.)	
		}

		// Render
		login.render();

	}

	//Lobby lobby;
	//lobby.addToPlayerList("An", sf::Color::Red, true);
	//lobby.addToPlayerList("Hoang", sf::Color::Blue, false);
	//lobby.addToPlayerList("Tuan", sf::Color::Cyan, false);
	//lobby.addToPlayerList("Minh", sf::Color::Magenta, false);
	//lobby.addToPlayerList("Tu", sf::Color::White, false);
	//lobby.addToPlayerList("Thanh", sf::Color::Yellow, false);

	////lobby.removeFromPlayerList("Tuan");

	//while (lobby.running()) {
	//	lobby.update();

	//	lobby.render();
	//}

	// End application
	return 0;
}
