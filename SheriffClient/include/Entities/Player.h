#ifndef PLAYER__
#define PLAYER__

#include <SFML/Graphics.hpp>

#pragma once
class Player
{
public:
	enum class MedalStatus {
		NONE,
		GOLD,
		SILVER,
		BLACK_MARKET,
		BLACK_MARKET_PLUS
	};

	Player(std::string name, sf::Color playerColor, bool isUserPlayer);
	~Player();

	bool initFontAndTexture();
	bool initUserPlayer(std::string name, sf::Color playerColor);
	bool initPlayer(float posX, float posY);

	std::string getPlayerName() const;
	bool setPlayerName(const std::string name);
	
	sf::Color getPlayerColor() const;
	bool setPlayerColor(const sf::Color color);

	sf::CircleShape& getAvatar();
	sf::CircleShape& getAvatarFrame();
	sf::Text& getNameText();

	sf::RectangleShape& getInfoTabIcon();
	sf::RectangleShape& getSheriffBadge();
	sf::RectangleShape& getBagIcon();
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

	int getPlayerGoodsAmount(int cardType);
	void increasePlayerGoodsAmount(int cardType, int value);
	void setPlayerGoodsAmount(int cardType, int value);

	int getPlayerMoney() const;
	void setPlayerMoney(int value);

	MedalStatus getPlayerMedalStatus(int cardType);
	void setPlayerMedalStatus(int cardType, MedalStatus status);

	int getPlayerScore() const;
	void setPlayerScore(int value);

private:
	sf::Font m_font;
	sf::CircleShape m_avatar;
	sf::Texture m_avatarTexture;
	sf::CircleShape m_avatarFrame;
	sf::Texture m_avatarFrameTexture;

	sf::Color m_playerColor;
	sf::Text m_nameText;

	sf::RectangleShape m_readyButton;
	sf::Text m_readyText;

	sf::RectangleShape m_sheriffBadge;
	sf::Texture m_sheriffBadgeTexture;

	sf::RectangleShape m_infoTabIcon;
	sf::Texture m_infoTabIconTexture;

	sf::RectangleShape m_bagIcon;
	sf::Texture m_bagIconTexture;

	sf::Text m_turnIndicator;

	bool m_isReady;
	bool m_isUserPlayer;

	// Game logic and stuff
	bool m_isSheriff;
	bool m_isInTurn;

	int m_goods[8];
	int m_money;

	MedalStatus m_medalStatus[8];
	int m_score;
};

#endif // !PLAYER__
