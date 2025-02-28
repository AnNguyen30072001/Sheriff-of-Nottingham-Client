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

class GameLogic;

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
		REVEAL,
		TIMEOUT_WITHDRAW,
		TIMEOUT_DISCARD
	};

	Game() = default;
	Game(std::vector<Player*> playerList);
	virtual ~Game();

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

	friend class GameLogic;

protected:
	// Init
	sf::RenderWindow* m_window;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;
	sf::Font m_font;
	sf::Clock m_clock;
	Popup* m_disconnectPopup;

	// Background
	sf::RectangleShape m_background;
	sf::Texture m_backgroundTexture;
	sf::RectangleShape m_cardsHolder;
	sf::Texture m_cardsHolderTexture;
	sf::RectangleShape m_withdrawEventMask;
	sf::RectangleShape m_discardEventMask;
	sf::RectangleShape m_SheriffEventMask;

	// Render Objects
	sf::RectangleShape m_ButtonLeft;
	sf::RectangleShape m_ButtonRight;
	sf::Text m_ButtonLeftText;
	sf::Text m_ButtonRightText;
	sf::RectangleShape m_moneyIcon;
	sf::Texture m_moneyIconTexture;
	sf::Text m_moneyText;
	sf::Text m_goodsReportText;
	sf::Text m_infoText;
	sf::Text m_guideText;

	// Shaders
	sf::Shader m_glowShader;

	// Players and Cards
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
	std::mutex m_textMutex;

	// Game Event
	Game::GameEvent m_gameEvent;
	Game::GameEvent m_lastUpdatedGameEvent;
	
	// Animation and Timer
	Timer* m_timer;
	AnimationManager m_animationPlayer;
	float m_elapsedTime;

	// Game Logic
	int m_MerchantShowingBagIndex;
	bool m_anyCardSelected;
	sf::Vector2f m_dragOffset;
	bool m_anyCardDragged;
	bool m_discardDone;
	bool m_revealingDone;

private:
	std::unique_ptr<GameLogic> m_gameLogic;

	void initVariables(std::vector<Player*> playerList);
	void initWindow();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool handleMouseDrag(sf::Vector2f mousePosXY);
	bool handleMouseHover(sf::Vector2f mousePosXY);
	bool handleMouseRelease();

};

#endif // !GAME__