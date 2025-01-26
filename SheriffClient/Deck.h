#ifndef _DECK__
#define _DECK__

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Card.h"

class Deck
{
public:
	const std::unordered_map<Card::CardType, std::string> m_cardNameToString = {
	{Card::APPLE, "Apple"},
	{Card::CHEESE, "Cheese"},
	{Card::BREAD, "Bread"},
	{Card::CHICKEN, "Chicken"},
	{Card::PEPPER, "Pepper"},
	{Card::MEAD, "Mead"},
	{Card::SILK, "Silk"},
	{Card::CROSSBOW, "Crossbow"}
	};

	Deck();
	~Deck();

	sf::RectangleShape getMainDeck() const;
	sf::RectangleShape getDiscardDeckLeft() const;
	sf::RectangleShape getDiscardDeckRight() const;

	bool setDiscardDeckLeftTexture(Card::CardType card);
	bool setDiscardDeckRightTexture(Card::CardType card);

private:
	

	sf::RectangleShape m_mainDeck;
	sf::RectangleShape m_discardDeckLeft;
	sf::RectangleShape m_discardDeckRight;

	sf::Texture m_mainDeckTexture;
	sf::Texture m_discardDeckLeftTexture;
	sf::Texture m_discardDeckRightTexture;

	bool initDeckUI();
};

#endif // !_DECK__




