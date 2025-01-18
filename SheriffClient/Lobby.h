#ifndef LOBBY__
#define LOBBY__

#include <SFML/Graphics.hpp>
#include "Player.h"

#pragma once
class Lobby
{
private:
	sf::RenderWindow* m_window;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;

	std::vector<Player*> m_playerList;

	bool initVariable();
	bool initWindow();

public:
	Lobby();
	~Lobby();

	// Accessors
	const bool running() const;
	
	// Function
	bool addToPlayerList(std::string name, sf::Color playerColor, bool isUserPlayer);
	bool removeFromPlayerList(std::string name);
	bool setupPlayerUI();

	bool handleMouseClick(sf::Vector2i mousePosXY);
	bool pollEvents();
	bool update();
	bool render();
};

#endif // !LOBBY__




