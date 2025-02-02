#ifndef _CARD__
#define _CARD__

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "Animation.h"

class Card
{
public:
	using Callback = std::function<void()>;
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
	void setupCardUI(float posX, float posY, sf::Vector2f scale = sf::Vector2f(1.f, 1.f));
	AnimationManager& getAnimationPlayer();

	void setSelected(const bool status);
	bool isSelected() const;

	void setDragging(const bool status);
	bool isDragging() const;

	void resetStaticPosition();

	bool animationMove(float durationSeconds, sf::Vector2f posEndValue, float scaleEndValue = 1.f, float delay = 0.f, Callback callback = nullptr);

private:
	CardType m_cardType;
	sf::RectangleShape m_card;
	sf::Texture m_cardTexture;
	sf::Vector2f m_staticPos;
	sf::Vector2f m_staticScale;

	bool m_isDragging;
	bool m_isSelected;
	AnimationManager m_animationPlayer;

	void initVariables();
};


#endif // !_CARD__




