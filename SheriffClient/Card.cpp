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

Card::Card(CardType cardType)
{
	m_cardType = cardType;
	initVariables();
	setCardTexture(cardType);
}

Card::~Card()
{

}

sf::RectangleShape Card::getCard() const
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

void Card::setSelected(const bool status)
{
	m_isSelected = status;
}

bool Card::isSelected() const
{
	return m_isSelected;
}

void Card::initVariables()
{
	m_card.setSize(sf::Vector2f(135.f, 195.f));
	m_card.setFillColor(sf::Color::White);
	m_card.setOutlineThickness(2.f);
	m_card.setOutlineColor(sf::Color::Black);

	m_isSelected = false;
}
