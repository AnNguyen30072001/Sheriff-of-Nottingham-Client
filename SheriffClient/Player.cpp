#include "Player.h"
#include <iostream>

Player::Player(std::string name, sf::Color playerColor, bool isUserPlayer)
{
	setPlayerName(name);
	setPlayerColor(playerColor);
	m_isUserPlayer = isUserPlayer;
	m_isReady = false;

	initFontAndTexture();
	if (isUserPlayer) {
		initUserPlayer(name, playerColor);
	}
}

Player::~Player()
{

}

bool Player::initFontAndTexture()
{
	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
		return false;
	}
	if (!m_avatarTexture.loadFromFile("Images/PlayerAvatar.png")) {
		std::cerr << "Error loading texture!\n";
		return false;
	}

	return true;
}

bool Player::initUserPlayer(std::string name, sf::Color playerColor)
{
	// User player setup
	m_avatar.setRadius(70.f);
	m_avatar.setFillColor(sf::Color::White);
	m_avatar.setTexture(&m_avatarTexture);
	m_avatar.setOutlineThickness(5.f);
	m_avatar.setOutlineColor(playerColor);

	m_nameText.setFont(m_font);
	m_nameText.setCharacterSize(24);
	m_nameText.setFillColor(sf::Color::White);
	m_nameText.setOutlineColor(sf::Color::Black);
	m_nameText.setOutlineThickness(2.f);
	m_nameText.setString(name);

	m_readyButton.setSize(sf::Vector2f(120.f, 50.f));
	m_readyButton.setFillColor(sf::Color(128, 128, 128)); // Gray by default
	m_readyButton.setOutlineColor(sf::Color::Black);
	m_readyButton.setOutlineThickness(1.f);

	m_readyText.setFont(m_font);
	m_readyText.setCharacterSize(18);
	m_readyText.setFillColor(sf::Color::White);
	m_readyText.setOutlineColor(sf::Color::Black);
	m_readyText.setOutlineThickness(2.f);
	m_readyText.setString("Ready");


	// Positioning
	m_avatar.setPosition(890.f, 800.f);
	m_nameText.setPosition(900.f, 765.f);
	m_readyButton.setPosition(1060.f, 810.f);
	m_readyText.setPosition(1095.f, 824.f);

	return true;
}

bool Player::initPlayer(float posX, float posY)
{
	m_avatar.setRadius(50.f);
	m_avatar.setFillColor(sf::Color::White);
	m_avatar.setTexture(&m_avatarTexture);
	m_avatar.setOutlineThickness(5.f);
	m_avatar.setOutlineColor(m_playerColor);

	m_nameText.setFont(m_font);
	m_nameText.setCharacterSize(20);
	m_nameText.setFillColor(sf::Color::White);
	m_nameText.setOutlineColor(sf::Color::Black);
	m_nameText.setOutlineThickness(2.f);

	m_readyButton.setSize(sf::Vector2f(60.f, 30.f));
	m_readyButton.setFillColor(sf::Color(128, 128, 128)); // Gray by default
	m_readyButton.setOutlineColor(sf::Color::Black);
	m_readyButton.setOutlineThickness(1.f);

	m_readyText.setFont(m_font);
	m_readyText.setCharacterSize(15);
	m_readyText.setFillColor(sf::Color::White);
	m_readyText.setOutlineColor(sf::Color::Black);
	m_readyText.setOutlineThickness(2.f);
	m_readyText.setString("Ready");

	m_isReady = false;

	// Positioning
	m_avatar.setPosition(posX, posY);
	m_nameText.setPosition(posX + 5, posY - 30.f);
	m_readyButton.setPosition(posX + 25.f, posY + 120.f);
	m_readyText.setPosition(
		m_readyButton.getPosition().x + 10.f,
		m_readyButton.getPosition().y + 6.f
	);

	return true;
}

std::string Player::getPlayerName() const
{
	return m_nameText.getString();
}

bool Player::setPlayerName(const std::string name)
{
	m_nameText.setString(name);
	return true;
}

sf::Color Player::getPlayerColor() const
{
	return m_playerColor;
}

bool Player::setPlayerColor(const sf::Color color)
{
	m_playerColor = color;
	return true;
}

sf::CircleShape Player::getAvatar() const
{
	return m_avatar;
}

sf::Text Player::getNameText() const
{
	return m_nameText;
}

bool Player::setReadyButtonAppearance(sf::Color color)
{
	m_readyButton.setFillColor(color);
	return true;
}

sf::RectangleShape Player::getReadyButton() const
{
	return m_readyButton;
}

sf::Text Player::getReadyText() const
{
	return m_readyText;
}

bool Player::setPlayerReady(bool status)
{
	m_isReady = status;
	return true;
}

bool Player::isPlayerReady()
{
	return m_isReady;
}

bool Player::isUserPlayer()
{
	return m_isUserPlayer;
}
