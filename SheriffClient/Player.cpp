#include "Player.h"
#include <iostream>

Player::Player(std::string name, sf::Color playerColor, bool isUserPlayer)
{
	setPlayerName(name);
	setPlayerColor(playerColor);
	m_isUserPlayer = isUserPlayer;
	m_isReady = false;
	m_isSheriff = false;
	m_isInTurn = false;
	m_money = 0;
	m_readyButton.setFillColor(sf::Color(128, 128, 128)); // Gray by default

	initFontAndTexture();
	if (isUserPlayer) {
		initUserPlayer(name, playerColor);
	}

	for (auto& good : m_goods) {
		good = 0;
	}
	//m_money = 0;
	//m_appleCount = 0;
	//m_chickenCount = 0;
	//m_cheeseCount = 0;
	//m_breadCount = 0;
	//m_crossbowCount = 0;
	//m_meadCount = 0;
	//m_pepperCount = 0;
	//m_silkCount = 0;
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
	if (!m_sheriffBadgeTexture.loadFromFile("Images/SheriffBadge.png")) {
		std::cerr << "Error loading texture!\n";
		return false;
	}
	if (!m_infoTabIconTexture.loadFromFile("Images/InfoTab.png")) {
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
	m_readyButton.setOutlineColor(sf::Color::Black);
	m_readyButton.setOutlineThickness(1.f);

	m_readyText.setFont(m_font);
	m_readyText.setCharacterSize(18);
	m_readyText.setFillColor(sf::Color::White);
	m_readyText.setOutlineColor(sf::Color::Black);
	m_readyText.setOutlineThickness(2.f);
	m_readyText.setString("Ready");

	m_infoTabIcon.setSize(sf::Vector2f(100, 100));
	m_infoTabIcon.setFillColor(sf::Color::White);
	m_infoTabIcon.setTexture(&m_infoTabIconTexture);

	m_sheriffBadge.setSize(sf::Vector2f(80, 80));
	m_sheriffBadge.setFillColor(sf::Color::White);
	m_sheriffBadge.setTexture(&m_sheriffBadgeTexture);

	m_turnIndicator.setFont(m_font);
	m_turnIndicator.setCharacterSize(24);
	m_turnIndicator.setFillColor(sf::Color::Green);
	m_turnIndicator.setOutlineColor(sf::Color::Black);
	m_turnIndicator.setOutlineThickness(2.f);
	m_turnIndicator.setString("In Turn");

	// Positioning
	m_avatar.setPosition(890.f, 870.f);
	m_nameText.setPosition(900.f, 835.f);

	m_readyButton.setPosition(1060.f, 880.f);
	m_readyText.setPosition(1095.f, 894.f);

	m_infoTabIcon.setPosition(740.f, 890.f);
	m_sheriffBadge.setPosition(1080.f, 900.f);
	m_turnIndicator.setPosition(922.f, 1025.f);

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
	m_readyButton.setOutlineColor(sf::Color::Black);
	m_readyButton.setOutlineThickness(1.f);

	m_readyText.setFont(m_font);
	m_readyText.setCharacterSize(15);
	m_readyText.setFillColor(sf::Color::White);
	m_readyText.setOutlineColor(sf::Color::Black);
	m_readyText.setOutlineThickness(2.f);
	m_readyText.setString("Ready");

	m_infoTabIcon.setSize(sf::Vector2f(75, 75));
	m_infoTabIcon.setFillColor(sf::Color::White);
	m_infoTabIcon.setTexture(&m_infoTabIconTexture);

	m_sheriffBadge.setSize(sf::Vector2f(60, 60));
	m_sheriffBadge.setFillColor(sf::Color::White);
	m_sheriffBadge.setTexture(&m_sheriffBadgeTexture);

	m_turnIndicator.setFont(m_font);
	m_turnIndicator.setCharacterSize(24);
	m_turnIndicator.setFillColor(sf::Color::Green);
	m_turnIndicator.setOutlineColor(sf::Color::Black);
	m_turnIndicator.setOutlineThickness(2.f);
	m_turnIndicator.setString("In Turn");

	// Positioning
	m_avatar.setPosition(posX, posY);
	m_nameText.setPosition(posX + 5, posY - 30.f);

	m_readyButton.setPosition(posX + 25.f, posY + 120.f);
	m_readyText.setPosition(
		m_readyButton.getPosition().x + 10.f,
		m_readyButton.getPosition().y + 6.f
	);

	m_infoTabIcon.setPosition(posX - 110.f, posY + 12.f);
	m_sheriffBadge.setPosition(posX + 135.f, posY + 20.f);
	m_turnIndicator.setPosition(posX + 12.f, posY + 115.f);

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

sf::CircleShape& Player::getAvatar()
{
	return m_avatar;
}

sf::Text& Player::getNameText()
{
	return m_nameText;
}

sf::RectangleShape& Player::getInfoTabIcon()
{
	return m_infoTabIcon;
}

sf::RectangleShape Player::getSheriffBadge() const
{
	return m_sheriffBadge;
}

sf::Text Player::getTurnIndicator() const
{
	return m_turnIndicator;
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

bool Player::setPlayerReady(const bool status)
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

bool Player::setSheriffStatus(const bool status)
{
	m_isSheriff = status;

	return true;
}

bool Player::isSheriff()
{
	return m_isSheriff;
}

bool Player::setTurn(const bool status)
{
	m_isInTurn = status;

	return true;
}

bool Player::isInTurn()
{
	return m_isInTurn;
}

int Player::getPlayerGoodsAmount(int cardType)
{
	return m_goods[cardType - 1];
}

void Player::increasePlayerGoodsAmount(int cardType, int value)
{
	m_goods[cardType - 1] += value;
}

int Player::getPlayerMoney() const
{
	return m_money;
}

void Player::setPlayerMoney(int value)
{
	m_money = value;
}
//
//int Player::getPlayerAppleCount() const
//{
//	return m_appleCount;
//}
//
//void Player::setPlayerAppleCount(int value)
//{
//	m_appleCount = value;
//}
//
//int Player::getPlayerChickenCount() const
//{
//	return m_chickenCount;
//}
//
//void Player::setPlayerChickenCount(int value)
//{
//	m_chickenCount = value;
//}
//
//int Player::getPlayerCheeseCount() const
//{
//	return m_cheeseCount;
//}
//
//void Player::setPlayerCheeseCount(int value)
//{
//	m_cheeseCount = value;
//}
//
//int Player::getPlayerBreadCount() const
//{
//	return m_breadCount;
//}
//
//void Player::setPlayerBreadCount(int value)
//{
//	m_breadCount = value;
//}
//
//int Player::getPlayerCrossbowCount() const
//{
//	return m_crossbowCount;
//}
//
//void Player::setPlayerCrossbowCount(int value)
//{
//	m_crossbowCount = value;
//}
//
//int Player::getPlayerSilkCount() const
//{
//	return m_silkCount;
//}
//
//void Player::setPlayerSilkCount(int value)
//{
//	m_silkCount = value;
//}
//
//int Player::getPlayerMeadCount() const
//{
//	return m_meadCount;
//}
//
//void Player::setPlayerMeadCount(int value)
//{
//	m_meadCount = value;
//}
//
//int Player::getPlayerPepperCount() const
//{
//	return m_pepperCount;
//}
//
//void Player::setPlayerPepperCount(int value)
//{
//	m_pepperCount = value;
//}
