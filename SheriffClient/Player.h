#ifndef PLAYER__
#define PLAYER__

#include <SFML/Graphics.hpp>

#pragma once
class Player
{
private:
	sf::Font m_font;
	sf::CircleShape m_avatar;  
	sf::Texture m_avatarTexture;
	sf::Color m_playerColor;
	sf::Text m_nameText;       

	sf::RectangleShape m_readyButton; 
	sf::Text m_readyText;  

	sf::RectangleShape m_sheriffBadge;
	sf::Texture m_sheriffBadgeTexture;

	sf::RectangleShape m_infoTabIcon;
	sf::Texture m_infoTabIconTexture;

	sf::Text m_turnIndicator;

	bool m_isReady;             
	bool m_isUserPlayer;

	// Game logic and stuff
	bool m_isSheriff;
	bool m_isInTurn;
	int m_money;
	int m_appleCount;
	int m_chickenCount;
	int m_cheeseCount;
	int m_breadCount;
	int m_crossbowCount;
	int m_meadCount;
	int m_pepperCount;
	int m_silkCount;

public:
	Player(std::string name, sf::Color playerColor, bool isUserPlayer);
	~Player();

	bool initFontAndTexture();
	bool initUserPlayer(std::string name, sf::Color playerColor);
	bool initPlayer(float posX, float posY);

	std::string getPlayerName() const;
	bool setPlayerName(const std::string name);
	
	sf::Color getPlayerColor() const;
	bool setPlayerColor(const sf::Color color);

	sf::CircleShape getAvatar() const;
	sf::Text getNameText() const;

	sf::RectangleShape getInfoTabIcon() const;
	sf::RectangleShape getSheriffBadge() const;
	sf::Text getTurnIndicator() const;

	bool setReadyButtonAppearance(sf::Color color);
	sf::RectangleShape getReadyButton() const;
	sf::Text getReadyText() const;

	bool setPlayerReady(const bool status);
	bool isPlayerReady();
	bool isUserPlayer();
	bool setSheriffStatus(const bool status);
	bool isSheriff();
	bool setTurn(const bool status);
	bool isInTurn();

	int getPlayerMoney() const;
	void setPlayerMoney(int value);

	int getPlayerAppleCount() const;
	void setPlayerAppleCount(int value);

	int getPlayerChickenCount() const;
	void setPlayerChickenCount(int value);

	int getPlayerCheeseCount() const;
	void setPlayerCheeseCount(int value);

	int getPlayerBreadCount() const;
	void setPlayerBreadCount(int value);

	int getPlayerCrossbowCount() const;
	void setPlayerCrossbowCount(int value);

	int getPlayerSilkCount() const;
	void setPlayerSilkCount(int value);

	int getPlayerMeadCount() const;
	void setPlayerMeadCount(int value);

	int getPlayerPepperCount() const;
	void setPlayerPepperCount(int value);
};

#endif // !PLAYER__
