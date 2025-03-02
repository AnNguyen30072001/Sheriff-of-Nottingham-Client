#ifndef _DECK__
#define _DECK__

#include <unordered_map>
#include <stack>
#include <SFML/Graphics.hpp>
#include "../include/Entities/Card.h"

class Deck
{
public:
	Deck();
	~Deck();

	sf::RectangleShape& getMainDeck();
	sf::RectangleShape& getDiscardDeckLeft();
	sf::RectangleShape& getDiscardDeckRight();

	std::stack<Card*>& getStackLeft();
	std::stack<Card*>& getStackRight();

	bool setDiscardDeckLeftTexture(Card::CardType card);
	bool setDiscardDeckRightTexture(Card::CardType card);
	bool setDiscardDeckLeftOutOfStockTexture();
	bool setDiscardDeckRightOutOfStockTexture();

private:
	sf::RectangleShape m_mainDeck;
	sf::RectangleShape m_discardDeckLeft;
	sf::RectangleShape m_discardDeckRight;

	//std::stack<Card*> m_cardsMainDeck;
	std::stack<Card*> m_cardsDeckLeft;
	std::stack<Card*> m_cardsDeckRight;

	sf::Texture m_mainDeckTexture;
	sf::Texture m_discardDeckLeftTexture;
	sf::Texture m_discardDeckRightTexture;
	sf::Texture m_outOfStockTexture;

	bool initDeckUI();
};

#endif // !_DECK__




