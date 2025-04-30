#ifndef _SUMMARY_
#define _SUMMARY_

#include <SFML/Graphics.hpp>
#include "../include/Entities/Player.h"

#define	GOODS_ICON_NUM_MAX				8
#define BLACK_MARKET_MEDAL_NUM_MAX		6

#pragma once
class Summary 
{
public:
	Summary(std::vector<Player*> playerList);
	~Summary();

	// Accessors
	const bool running() const;

	//Functions
	bool pollEvents();
	bool update();
	bool render();

private:
	void initVariables(std::vector<Player*> playerList);
	void initWindow();

	// Init
	sf::RenderWindow*				m_window;
	sf::VideoMode					m_videoMode;
	sf::Event						m_ev;
	sf::Font						m_font;

	// Background
	sf::RectangleShape				m_background;
	sf::Texture						m_backgroundTexture;

	// Objects
	sf::Text						m_scoreText;
	sf::RectangleShape				m_scoreTextBox;

	sf::RectangleShape				m_moneyIcon;
	sf::Texture						m_moneyIconTexture;
	sf::Text						m_moneyText;
	sf::RectangleShape				m_moneyTextBox;

	sf::RectangleShape				m_goodsIcon[GOODS_ICON_NUM_MAX];
	sf::Texture						m_goodsIconTexture[GOODS_ICON_NUM_MAX];
	sf::Text						m_goodsAmountText[GOODS_ICON_NUM_MAX];
	sf::RectangleShape				m_goodsAmountTextBox[GOODS_ICON_NUM_MAX];

	sf::RectangleShape				m_blackMarketMedals[BLACK_MARKET_MEDAL_NUM_MAX];			// Black market medals for contrabands
	sf::Texture						m_blackMarketMedalTexture[BLACK_MARKET_MEDAL_NUM_MAX];

	std::vector<Player*>			m_playerList;
	int								m_highlightedPlayerIdx;

	bool handleMouseClick(sf::Vector2f mousePosXY);

	void setPlayersLayout();
	void setPlayerInfoLayout(int index);

	void updatePlayersMedalStatus();
	void updatePlayerScore(Player* player);

	void centerText(sf::Text & text, sf::RectangleShape & boundingBox);
};


#endif // !_SUMMARY_
