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
		IDLE,
		DEFAULT,
		DISCARD,
		WITHDRAW,
		PRESENT,
		SHERIFF_TURN
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
	GameEvent& getGameEvent();

	bool pollEvents();
	bool update();
	bool render();

	bool setupPlayerUI();
	void userHandUI();
	void updateUserHandAnimation(float deltaTime);

	void onMessageReceived(const nlohmann::json& jsonMessage);

private:
	// Initial stuffs
	sf::RenderWindow* m_window;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;
	sf::Font m_font;
	sf::Clock m_clock;

	// Background static stuffs
	sf::RectangleShape m_background;
	sf::Texture m_backgroundTexture;
	sf::RectangleShape m_withdrawEventMask;
	sf::RectangleShape m_discardEventMask;
	sf::RectangleShape m_SheriffEventMask;

	// Objects
	sf::RectangleShape m_ButtonLeft;
	sf::RectangleShape m_ButtonRight;
	sf::Text m_ButtonLeftText;
	sf::Text m_ButtonRightText;
	sf::RectangleShape m_moneyIcon;
	sf::Texture m_moneyIconTexture;
	sf::Text m_BribeAmountText;
	sf::Text m_goodsReportText;

	// Card stuffs
	std::vector<Card*> m_userHand;
	std::vector<Card*> m_selectedCards;
	std::vector<Player*> m_playerList;
	Deck* m_deck;
	Tablet* m_tablet;
	std::vector<Card*> m_dummyCards;		// Dummy cards for animation render and temporary displays

	// Game logic stuff
	sf::Vector2f m_dragOffset;
	Game::GameEvent m_gameEvent;
	AnimationManager m_animationPlayer;
	Timer* m_timer;
	Card::CardType m_goodsReport;
	int	m_bribeAmount;
	int m_MerchantShowingBagIndex;
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
	void handleOpponentWithdrawEvent(PileType type, int playerIndex);
	void handleOpponentDiscardEvent(PileType type, int playerIndex, Card::CardType cardType);
};

#endif // !GAME__