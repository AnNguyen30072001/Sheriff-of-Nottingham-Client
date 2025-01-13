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
	Login login;

	while (login.running()) {
		// Update
		login.update();

		// Render
		login.render();

	}

	// End application
	return 0;
}
