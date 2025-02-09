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
#include "Popup.h"

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
		DISCONNECTED,
		IDLE,
		DEFAULT,
		DISCARD,
		WITHDRAW,
		PRESENT,
		SHERIFF_TURN,
		REVEAL
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
	Popup* m_disconnectPopup;

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
	sf::Text m_moneyText;
	sf::Text m_goodsReportText;
	sf::Text m_infoText;

	// Card stuffs
	std::vector<Card*> m_userHand;
	std::vector<Card*> m_selectedCards;
	std::vector<Player*> m_playerList;
	Deck* m_deck;
	Tablet* m_tablet;
	std::vector<Card*> m_dummyCards;		// Dummy cards for animation render and temporary displays

	// Threading
	std::mutex m_userHandMutex;
	std::mutex m_selectedCardsMutex;
	std::mutex m_dummyCardsMutex;

	// Game logic stuff
	sf::Vector2f m_dragOffset;
	Game::GameEvent m_gameEvent;
	Game::GameEvent m_lastUpdatedGameEvent;
	AnimationManager m_animationPlayer;
	Timer* m_timer;
	Card::CardType m_goodsReport;
	float m_elapsedTime;
	int	m_bribeAmount;
	int m_MerchantShowingBagIndex;
	bool m_anyCardSelected;
	bool m_anyCardDragged;
	bool m_discardDone;
	bool m_revealingDone;

	void initVariables(std::vector<Player*> playerList);
	void initWindow();

	bool attemptReconnect(float deltaTime);

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool handleMouseDrag(sf::Vector2f mousePosXY);
	bool handleMouseRelease();
	void handlePresentEvent();
	void handleWithdrawEvent(PileType type);
	void handleDiscardEvent(PileType type, std::string cardName);
	void handleOpponentWithdrawEvent(PileType type, int playerIndex);
	void handleOpponentDiscardEvent(PileType type, int playerIndex, Card::CardType cardType);
	void handleSheriffInspectEvent(const nlohmann::json& jsonMessage);
	void handleSheriffPassEvent(const nlohmann::json& jsonMessage);
	void revealCard(Player* sheriff, std::vector<Card::CardType> cardTypes, int revealIndex, const nlohmann::json& jsonMessage);
	void retrieveCards(const nlohmann::json& jsonMessage);
};

#endif // !GAME__