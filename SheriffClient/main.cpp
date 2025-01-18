#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "Game.h"
#include "Lobby.h"
#include "Login.h"

int main() 
{
	//Login login;

	//while (login.running()) {
	//	// Update
	//	login.update();

	//	// Render
	//	login.render();

	//}

	Lobby lobby;
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
