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
	static const std::unordered_map<CardType, std::string> m_cardNameToString;
	static const std::unordered_map<std::string, CardType> m_stringToCardName;

	Card(CardType cardType);
	~Card();

	sf::RectangleShape& getCard();
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




