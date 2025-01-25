#ifndef GAME__
#define GAME__

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Network.h"

#pragma once
class Game : public Observer
{
private:
	sf::RenderWindow* m_window;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;

	sf::RectangleShape m_background;
	sf::Texture m_backgroundTexture;
	sf::RectangleShape m_mainDeck;
	sf::Texture m_mainDeckTexture;

	std::vector<Player*> m_playerList;


	void initVariables(std::vector<Player*> playerList);
	void initWindow();

public:
	Game(std::vector<Player*> playerList);
	~Game();

	// Accessors
	const bool running() const;

	// Function
	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool pollEvents();
	bool update();
	bool render();

	bool setupPlayerUI();


	void onMessageReceived(const std::string& msg);
};

#endif // !GAME__