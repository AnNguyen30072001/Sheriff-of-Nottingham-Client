#ifndef PLAYER__
#define PLAYER__

#include <SFML/Graphics.hpp>

#pragma once
class Player
{
private:
	sf::CircleShape m_avatar;  
	sf::Color m_playerColor;
	sf::Text m_nameText;        
	sf::RectangleShape m_readyButton; 
	sf::Text m_readyText;       
	bool m_isReady;             
	bool m_isUserPlayer;	
	sf::Font m_font;
	sf::Texture m_avatarTexture;

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
	bool setReadyButtonAppearance(sf::Color color);
	sf::RectangleShape getReadyButton() const;
	sf::Text getReadyText() const;

	bool setPlayerReady(bool status);
	bool isPlayerReady();
	bool isUserPlayer();
};

#endif // !PLAYER__
