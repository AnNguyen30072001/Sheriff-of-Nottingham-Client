#include <iostream>
#include "Deck.h"

Deck::Deck()
{
	initDeckUI();
}

Deck::~Deck()
{

}

sf::RectangleShape& Deck::getMainDeck()
{
	return m_mainDeck;
}

sf::RectangleShape& Deck::getDiscardDeckLeft()
{
	return m_discardDeckLeft;
}

sf::RectangleShape& Deck::getDiscardDeckRight()
{
	return m_discardDeckRight;
}

std::stack<Card*>& Deck::getStackLeft()
{
	return m_cardsDeckLeft;
}

std::stack<Card*>& Deck::getStackRight()
{
	return m_cardsDeckRight;
}

bool Deck::setDiscardDeckLeftTexture(Card::CardType card)
{
	std::string texturePath = "Images/" + Card::m_cardNameToString.at(card) + ".png";
	if (!m_discardDeckLeftTexture.loadFromFile(texturePath)) {
		std::cerr << "Error loading deck texture!" << std::endl;
		return false;
	}
	m_discardDeckLeft.setTexture(&m_discardDeckLeftTexture, true);

	return true;
}

bool Deck::setDiscardDeckRightTexture(Card::CardType card)
{
	std::string texturePath = "Images/" + Card::m_cardNameToString.at(card) + ".png";
	if (!m_discardDeckRightTexture.loadFromFile(texturePath)) {
		std::cerr << "Error loading deck texture!" << std::endl;
		return false;
	}
	m_discardDeckRight.setTexture(&m_discardDeckRightTexture, true);

	return true;
}

bool Deck::setDiscardDeckLeftOutOfStockTexture()
{
	m_discardDeckLeft.setTexture(&m_outOfStockTexture, true);

	return true;
}

bool Deck::setDiscardDeckRightOutOfStockTexture()
{
	m_discardDeckRight.setTexture(&m_outOfStockTexture, true);

	return true;
}

bool Deck::initDeckUI()
{
	if (!m_mainDeckTexture.loadFromFile("Images/MainDeck_2.png")) {
		std::cerr << "Error loading deck texture!" << std::endl;
	}
	if (!m_outOfStockTexture.loadFromFile("Images/OutOfStock.png")) {
		std::cerr << "Error loading deck texture!" << std::endl;
	}

	m_mainDeck.setSize(sf::Vector2f(150.f, 200.f));
	m_mainDeck.setFillColor(sf::Color::White);
	m_mainDeck.setOutlineThickness(1.f);
	m_mainDeck.setOutlineColor(sf::Color::Black);
	m_mainDeck.setTexture(&m_mainDeckTexture);

	m_discardDeckLeft.setSize(sf::Vector2f(108.f, 156.f));
	m_discardDeckLeft.setFillColor(sf::Color::White);
	m_discardDeckLeft.setOutlineThickness(2.f);
	m_discardDeckLeft.setOutlineColor(sf::Color::Black);

	m_discardDeckRight.setSize(sf::Vector2f(108.f, 156.f));
	m_discardDeckRight.setFillColor(sf::Color::White);
	m_discardDeckRight.setOutlineThickness(2.f);
	m_discardDeckRight.setOutlineColor(sf::Color::Black);

	// Positioning
	m_mainDeck.setPosition(885.f, 280.f);
	m_discardDeckLeft.setPosition(650.f, 324.f);
	m_discardDeckRight.setPosition(1162.f, 324.f);

	return true;
}
