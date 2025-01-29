#ifndef LOBBY__
#define LOBBY__

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Network.h"

#pragma once
class Lobby : public Observer
{
private:
	sf::RenderWindow* m_window;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;
	sf::RectangleShape m_background;
	sf::Texture m_backgroundTexture;

	std::vector<Player*> m_playerList;
	uint8_t m_newPlayerIdx;

	bool initVariable();
	bool initWindow();

public:
	enum insertPos {
		TAIL,
		FRONT
	};

	Lobby();
	~Lobby();

	// Accessors
	const bool running() const;
	
	// Function
	std::vector<Player*> getPlayerList() const;
	bool addToPlayerList(std::string name, sf::Color playerColor, bool isUserPlayer, Lobby::insertPos insertPos = TAIL);
	bool removeFromPlayerList(std::string name);
	bool setupPlayerUI();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool pollEvents();
	bool update();
	bool render();

	void onMessageReceived(const nlohmann::json& jsonMessage);
};

#endif // !LOBBY__




