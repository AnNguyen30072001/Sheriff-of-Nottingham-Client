#include <iostream>
#include "Card.h"

const std::unordered_map<Card::CardType, std::string> Card::m_cardNameToString = {
	{APPLE, "Apple"},
	{CHEESE, "Cheese"},
	{BREAD, "Bread"},
	{CHICKEN, "Chicken"},
	{PEPPER, "Pepper"},
	{MEAD, "Mead"},
	{SILK, "Silk"},
	{CROSSBOW, "Crossbow"}
};

const std::unordered_map<std::string, Card::CardType> Card::m_stringToCardName = {
	{"Apple", Card::APPLE},
	{"Cheese", Card::CHEESE},
	{"Bread", Card::BREAD},
	{"Chicken", Card::CHICKEN},
	{"Pepper", Card::PEPPER},
	{"Mead", Card::MEAD},
	{"Silk", Card::SILK},
	{"Crossbow", Card::CROSSBOW}
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
	m_card.setTexture(&m_cardTexture);
}

void Card::setupCardUI(float posX, float posY)
{
	m_card.setPosition(posX, posY);
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

bool Card::animationMove(float durationSeconds, sf::Vector2f posEndValue, float scaleEndValue, Callback callback)
{
	m_animationPlayer.addAnimation(new Animation(m_card, Animation::Type::MOVE_AND_SCALE, durationSeconds, posEndValue, scaleEndValue, callback));

	return true;
}

void Card::initVariables()
{
	m_card.setSize(sf::Vector2f(135.f, 195.f));
	m_card.setFillColor(sf::Color::White);
	m_card.setOutlineThickness(2.f);
	m_card.setOutlineColor(sf::Color::Black);

	m_isSelected = false;
}
