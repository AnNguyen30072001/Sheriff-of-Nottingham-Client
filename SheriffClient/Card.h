#ifndef _CARD__
#define _CARD__

#include <SFML/Graphics.hpp>
#include <unordered_map>

class Card
{
public:
	enum CardType {
		INVALID,
		APPLE,
		BREAD,
		CHEESE,
		CHICKEN,
		CROSSBOW,
		MEAD,
		PEPPER,
		SILK
	};
	const std::unordered_map<CardType, std::string> m_cardNameToString = {
	{APPLE, "Apple"},
	{CHEESE, "Cheese"},
	{BREAD, "Bread"},
	{CHICKEN, "Chicken"},
	{PEPPER, "Pepper"},
	{MEAD, "Mead"},
	{SILK, "Silk"},
	{CROSSBOW, "Crossbow"}
	};

	Card(CardType cardType);
	~Card();

	sf::RectangleShape getCard() const;
	CardType getCardType() const;
	void setCardTexture(CardType cardType);
	void setupCardUI(float posX, float posY);

	void setSelected(const bool status);
	bool isSelected() const;

private:
	CardType m_cardType;
	sf::RectangleShape m_card;
	sf::Texture m_cardTexture;
	bool m_isSelected;

	void initVariables();
};

#endif // !_CARD__




