#ifndef GAME__
#define GAME__

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Player.h"
#include "Card.h"
#include "Deck.h"
#include "Tablet.h"
#include "Network.h"
#include "Animation.h"
#include "Timer.h"

#pragma once
class Game : public Observer
{
public:
	enum class PileType {
		LEFT_DISCARD_PILE,
		RIGHT_DISCARD_PILE,
		MAIN_DECK
	};
	enum GameEvent {
		DEFAULT,
		DISCARD,
		WITHDRAW,
		PRESENT
	};

	Game(std::vector<Player*> playerList);
	~Game();

	// Accessors
	const bool running() const;

	// Function
	bool addToUserHand(Card::CardType card);
	bool removeFromUserHand(uint8_t index);
	std::vector<Card*>& getSelectedCards();
	Deck* getDeck();

	bool pollEvents();
	bool update();
	bool render();

	bool setupPlayerUI();
	void userHandUI();
	void updateUserHandAnimation(float deltaTime);

	void onMessageReceived(const nlohmann::json& jsonMessage);

private:
	sf::RenderWindow* m_window;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;
	sf::Font m_font;
	sf::Clock m_clock;
	sf::Vector2f m_dragOffset;
	Game::GameEvent m_gameEvent;
	AnimationManager m_animationPlayer;
	Timer* m_timer;

	// Background static stuff
	sf::RectangleShape m_background;
	sf::Texture m_backgroundTexture;
	sf::RectangleShape m_withdrawEventMask;
	sf::RectangleShape m_discardEventMask;

	sf::RectangleShape m_ButtonLeft;
	sf::RectangleShape m_ButtonRight;
	sf::Text m_ButtonLeftText;
	sf::Text m_ButtonRightText;

	std::vector<Card*> m_userHand;
	std::vector<Card*> m_selectedCards;
	std::vector<Player*> m_playerList;
	Deck* m_deck;
	Tablet* m_tablet;

	bool m_anyCardSelected;
	bool m_anyCardDragged;

	void initVariables(std::vector<Player*> playerList);
	void initWindow();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool handleMouseDrag(sf::Vector2f mousePosXY);
	bool handleMouseRelease();
	void handlePresentEvent();
	void handleWithdrawEvent(PileType type);
	void handleDiscardEvent(PileType type, std::string cardName);
};

#endif // !GAME__