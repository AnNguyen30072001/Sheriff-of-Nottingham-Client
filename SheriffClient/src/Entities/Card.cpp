#include <iostream>
#include "../include/Entities/Card.h"

const std::unordered_map<Card::CardType, std::string> Card::m_cardNameToString = {
	{APPLE, "Apple"},
	{CHEESE, "Cheese"},
	{BREAD, "Bread"},
	{CHICKEN, "Chicken"},
	{PEPPER, "Pepper"},
	{MEAD, "Mead"},
	{SILK, "Silk"},
	{CROSSBOW, "Crossbow"},
	{UNKNOWN, "Unknown"}
};

const std::unordered_map<std::string, Card::CardType> Card::m_stringToCardName = {
	{"Apple", APPLE},
	{"Cheese", CHEESE},
	{"Bread", BREAD},
	{"Chicken", CHICKEN},
	{"Pepper", PEPPER},
	{"Mead", MEAD},
	{"Silk", SILK},
	{"Crossbow", CROSSBOW},
	{"Unknown", UNKNOWN}
};

const std::unordered_map<Card::CardType, int> Card::cardTypeToValue = {
	{APPLE, 2},
	{CHEESE, 3},
	{BREAD, 3},
	{CHICKEN, 4},
	{PEPPER, 6},
	{MEAD, 7},
	{SILK, 8},
	{CROSSBOW, 9},
	{UNKNOWN, 0}
};

const std::unordered_map<Card::CardType, int> Card::cardTypeToPenalty = {
	{APPLE, 2},
	{CHEESE, 2},
	{BREAD, 2},
	{CHICKEN, 2},
	{PEPPER, 4},
	{MEAD, 4},
	{SILK, 4},
	{CROSSBOW, 4},
	{UNKNOWN, 0}
};

const std::unordered_map<Card::CardType, int> Card::cardTypeToGoldBonus = {
	{APPLE, 20},
	{CHEESE, 15},
	{BREAD, 15},
	{CHICKEN, 10},
	{UNKNOWN, 0}
};

const std::unordered_map<Card::CardType, int> Card::cardTypeToSilverBonus = {
	{APPLE, 10},
	{CHEESE, 10},
	{BREAD, 10},
	{CHICKEN, 5},
	{UNKNOWN, 0}
};

const std::unordered_map<Card::CardType, int> Card::cardTypeToContrabandBonus = {
	{PEPPER, 30},
	{MEAD, 32},
	{SILK, 34},
	{CROSSBOW, 0},
	{UNKNOWN, 0}
};

Card::Card(CardType cardType)
{
	m_cardType = cardType;
	initVariables();
	setCardTexture(cardType);
}

Card::~Card()
{

}

sf::RectangleShape& Card::getCard()
{
	return m_card;
}

Card::CardType Card::getCardType() const
{
	return m_cardType;
}

void Card::setCardTexture(CardType cardType)
{
	std::string texturePath = "Images/" + m_cardNameToString.at(cardType) + ".png";
	if (!m_cardTexture.loadFromFile(texturePath)) {
		std::cerr << "Error loading card texture!" << std::endl;
	}
	m_card.setTexture(&m_cardTexture, true);
	m_cardType = cardType;
}

void Card::setupCardUI(float posX, float posY, sf::Vector2f scale)
{
	m_card.setPosition(posX, posY);
	m_staticPos = sf::Vector2f(posX, posY);
	m_staticScale = scale;
}

AnimationManager & Card::getAnimationPlayer()
{
	return m_animationPlayer;
}

void Card::setSelected(const bool status)
{
	m_isSelected = status;
}

bool Card::isSelected() const
{
	return m_isSelected;
}

void Card::setDragging(const bool status)
{
	m_isDragging = status;
}

bool Card::isDragging() const
{
	return m_isDragging;
}

void Card::setHovering(const bool status)
{
	m_isHovered = status;
}

bool Card::isHovered() const
{
	return m_isHovered;
}

void Card::resetStaticPosition()
{
	m_card.setPosition(m_staticPos);
	m_card.setScale(m_staticScale);
}

bool Card::animationMove(float durationSeconds, sf::Vector2f posEndValue, float scaleEndValue, float delay, Callback callback)
{
	m_animationPlayer.addAnimation(new Animation(m_card, Animation::Type::MOVE_AND_SCALE, durationSeconds, posEndValue, scaleEndValue, delay, callback));

	return true;
}

void Card::initVariables()
{
	m_card.setSize(sf::Vector2f(135.f, 195.f));
	m_card.setFillColor(sf::Color::White);
	m_card.setOutlineThickness(2.f);
	m_card.setOutlineColor(sf::Color::Black);

	m_isSelected = false;
	m_isDragging = false;
	m_isHovered = false;
	m_staticPos = sf::Vector2f(0.f, 0.f);
	m_staticScale = sf::Vector2f(1.f, 1.f);

	// If card type is unknown, initially hide the card
	if (m_cardType == UNKNOWN) {
		m_card.setScale(0.f, 0.f);
	}
}
