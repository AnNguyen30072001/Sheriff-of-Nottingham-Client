#include "game.h"
#include <Windows.h>

void Game::initVariables(std::vector<Player*> playerList)
{
	m_gameEvent = Game::DEFAULT;
	m_anyCardSelected = false;
	m_window = nullptr;
	if (!m_backgroundTexture.loadFromFile("Images/Background.png")) {
		std::cerr << "Error loading background texture!";
	}
	m_backgroundTexture.setSmooth(true);

	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	m_playerList = playerList;
	m_deck = nullptr;
	m_tablet = nullptr;
	m_timer = nullptr;
}

void Game::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	m_deck = new Deck();
	m_tablet = new Tablet(m_window);
	m_timer = new Timer(m_window);

	// For testing only
	m_deck->getStackLeft().push(new Card(Card::PEPPER));
	m_deck->getStackLeft().push(new Card(Card::PEPPER));
	m_deck->getStackLeft().push(new Card(Card::CHICKEN));
	m_deck->getStackLeft().push(new Card(Card::APPLE));
	m_deck->getStackRight().push(new Card(Card::APPLE));
	m_deck->getStackRight().push(new Card(Card::APPLE));
	m_deck->getStackRight().push(new Card(Card::BREAD));
	m_deck->getStackRight().push(new Card(Card::CROSSBOW));
	m_deck->setDiscardDeckLeftTexture(Card::APPLE);
	m_deck->setDiscardDeckRightTexture(Card::CROSSBOW);
	// End of Test

	// Backgrounds
	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setFillColor(sf::Color::White);
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));
	m_background.setTexture(&m_backgroundTexture);
	m_withdrawEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_withdrawEventMask.setFillColor(sf::Color(0, 0, 0, 180));

	// Configure the button left
	m_ButtonLeft.setSize(sf::Vector2f(170.f, 70.f));
	m_ButtonLeft.setFillColor(sf::Color::Red);
	m_ButtonLeft.setOutlineColor(sf::Color(30, 30, 30)); // Dark Gray
	m_ButtonLeft.setOutlineThickness(3.f);
	// Configure the text
	m_ButtonLeftText.setFont(m_font);
	m_ButtonLeftText.setString("Discard");
	m_ButtonLeftText.setCharacterSize(28); 
	m_ButtonLeftText.setFillColor(sf::Color::White);
	// Center the text within the button
	sf::FloatRect textBoundsLeft = m_ButtonLeftText.getLocalBounds();
	m_ButtonLeftText.setOrigin(textBoundsLeft.left + textBoundsLeft.width / 2, textBoundsLeft.top + textBoundsLeft.height / 2);

	// Configure the button right
	m_ButtonRight.setSize(sf::Vector2f(170.f, 70.f));
	m_ButtonRight.setFillColor(sf::Color(100, 149, 237)); // Cornflower Blue
	m_ButtonRight.setOutlineColor(sf::Color(30, 30, 30)); // Dark Gray
	m_ButtonRight.setOutlineThickness(3.f);
	// Configure the text
	m_ButtonRightText.setFont(m_font);
	m_ButtonRightText.setString("Present");
	m_ButtonRightText.setCharacterSize(28);
	m_ButtonRightText.setFillColor(sf::Color::White);
	// Center the text within the button
	sf::FloatRect textBoundsRight = m_ButtonRightText.getLocalBounds();
	m_ButtonRightText.setOrigin(textBoundsRight.left + textBoundsRight.width / 2, textBoundsRight.top + textBoundsRight.height / 2);

	// Positioning buttons
	m_ButtonLeft.setPosition(500.f, 905.f);
	m_ButtonRight.setPosition(1245.f, 905.f);
	m_ButtonLeftText.setPosition(m_ButtonLeft.getPosition().x + m_ButtonLeft.getSize().x / 2,
		m_ButtonLeft.getPosition().y + m_ButtonLeft.getSize().y / 2);
	m_ButtonRightText.setPosition(m_ButtonRight.getPosition().x + m_ButtonRight.getSize().x / 2,
		m_ButtonRight.getPosition().y + m_ButtonRight.getSize().y / 2);

	// Initial draw user hand
	userHandUI();
}

void Game::handlePresentEvent()
{
	// Get infomation
	std::map<std::string, int> cardCount;
	for (auto& userCard : m_userHand) {
		if (userCard->isSelected()) {
			cardCount[Card::m_cardNameToString.at(userCard->getCardType())]++;
		}
	}

	// Prepare JSON message
	json message;
	message["MessageType"] = "MERCHANT_GIVE_BAG";
	message["PlayerName"] = m_playerList[0]->getPlayerName();
	// Add "Bag" section with card counts
	for (auto it = cardCount.begin(); it != cardCount.end(); ++it) {
		message["Bag"][it->first] = it->second;
	}
	message["Report"] = Card::m_cardNameToString.at(m_tablet->getPresentedGoods());
	message["Fee"] = m_tablet->getBribeAmount();

	// Convert message and send
	std::string messageString = message.dump();
	Network::sendMessage(messageString);

	// Reset game event
	m_tablet->resetOptions();
	m_gameEvent = Game::DEFAULT;
}

Game::Game(std::vector<Player*> playerList)
{
	initVariables(playerList);
	initWindow();
}

Game::~Game()
{
	for (int i = 0; i < m_playerList.size(); i++) {
		delete m_playerList[i];
	}
	for (int i = 0; i < m_userHand.size(); i++) {
		delete m_userHand[i];
	}
	for (auto& player : m_playerList) {
		delete player;
	}
	delete m_deck;
	delete m_tablet;
	delete m_timer;
	delete this->m_window;
}

const bool Game::running() const
{
	return m_window->isOpen();
}

bool Game::addToUserHand(Card::CardType card)
{
	if (m_userHand.size() >= 6U) return false;

	m_userHand.push_back(new Card(card));
	//userHandUI();

	return true;
}

bool Game::removeFromUserHand(uint8_t index)
{
	m_userHand.erase(m_userHand.begin() + index);

	return true;
}

bool Game::handleMouseClick(sf::Vector2f mousePosXY)
{
	// Select cards to discard or present
	m_anyCardSelected = false;
	for (int i = 0; i < m_userHand.size(); i++) {
		if (m_userHand[i]->getCard().getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_gameEvent == DEFAULT) {
			m_userHand[i]->setSelected(!m_userHand[i]->isSelected());
			std::cout << "Card " + Card::m_cardNameToString.at(m_userHand[i]->getCardType()) + ": " + (m_userHand[i]->isSelected() ? "Selected\n" : "Not selected\n");

			float posX = 520.f + (i % 6) * 150.f;
			m_userHand[i]->animationMove(0.1,
				sf::Vector2f(posX, m_userHand[i]->isSelected() ? 600.f : 635.f));
		}
		
		// Check if any card is selected for other interactions
		if (m_userHand[i]->isSelected()) {
			m_anyCardSelected = true;
		}
	}

	// If any card is selected and it is user's merchant turn, the Discard and Present buttons are interactable
	if (m_anyCardSelected && m_playerList[0]->isInTurn() && !m_playerList[0]->isSheriff() && m_gameEvent == DEFAULT) {
		// Onclick discard event
		if (m_ButtonLeft.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			// Move presented cards from user hand to discard area
			for (int i = 0; i < m_userHand.size(); i++) {
				if (m_userHand[i]->isSelected()) {
					m_selectedCards.push_back(m_userHand[i]);
					removeFromUserHand(i);
					i--;
				}
			}
			for (int i = 0; i < m_selectedCards.size(); i++) {
				float posX = 78.f + (i % 3) * 105.f;
				float posY = i < 3 ? 350.f : 505.f;
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::MOVE_AND_SCALE, 
					0.5, sf::Vector2f(posX, posY), 0.72));
			}
			// Re-arrange userhand
			userHandUI();
			// Change state machine
			m_gameEvent = WITHDRAW;
		}

		// Onclick present event
		if (m_ButtonRight.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			m_tablet->showTablet(Tablet::Type::GIVE_BAG, m_playerList[0]->getPlayerMoney());
			m_gameEvent = Game::PRESENT;
		}
	}

	// If it is user's sheriff turn, the Inspect and Pass buttons are interactable
	if (m_playerList[0]->isSheriff() && m_playerList[0]->isInTurn() && m_gameEvent == DEFAULT) {

	}

	// If a player's catalog is clicked, show that player's info
	for (auto& player : m_playerList) {
		if (player->getInfoTabIcon().getGlobalBounds().contains(mousePosXY) && m_gameEvent == DEFAULT) {
			m_tablet->showTablet(Tablet::Type::INFO, player->getPlayerMoney(), player);
		}
	}

	// Left deck is clicked when withdrawing
	if (m_deck->getDiscardDeckLeft().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[0]->getPlayerName();
		message["Pile"] = "LEFT_DISCARD_DECK";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
	}

	// Right deck is clicked when withdrawing
	if (m_deck->getDiscardDeckRight().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[0]->getPlayerName();
		message["Pile"] = "RIGHT_DISCARD_DECK";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
	}

	// Main deck is clicked when withdrawing
	if (m_deck->getMainDeck().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[0]->getPlayerName();
		message["Pile"] = "MAIN_DECK";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
	}

	return true;
}

bool Game::pollEvents()
{
	while (m_window->pollEvent(m_ev)) {
		switch (m_ev.type) {
		case sf::Event::Closed:
			m_window->close();
			break;

		case sf::Event::MouseButtonPressed:
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_window);
				sf::Vector2f mousePosXY = m_window->mapPixelToCoords(mousePosXYLocal);
				handleMouseClick(mousePosXY);
			}
			break;
		default:
			break;
		}
	}

	return true;
}

bool Game::update()
{
	float deltaTime = m_clock.restart().asSeconds();
	// Update animation if needed
	m_animationPlayer.update(deltaTime);
	updateUserHandAnimation(deltaTime);

	// Update the timer if needed
	m_timer->update();

	switch (m_gameEvent)
	{
	case Game::DEFAULT:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		setupPlayerUI();

		// If tablet is shown, it is interactable
		m_tablet->update();

		break;

	case Game::DISCARD:


		break;

	case Game::WITHDRAW:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		setupPlayerUI();

		break;

	case Game::PRESENT:
		if (m_tablet->isTabletVisible()) {
			// If tablet is shown, it is interactable
			m_tablet->update();

			if (m_tablet->isPresentConfirmed()) {
				// Player confirm present options. Handle logic and then reset tablet options
				handlePresentEvent();
			}
		}
		else {
			m_gameEvent = DEFAULT;
		}
		break;

	default:
		break;
	}

	return true;
}

bool Game::render()
{
	m_window->clear();

	// Background
	m_window->draw(m_background);

	// Player avatar and stuff
	for (const auto& player : m_playerList) {
		m_window->draw(player->getAvatar());
		m_window->draw(player->getNameText());
		m_window->draw(player->getInfoTabIcon());
		if (player->isSheriff()) {
			m_window->draw(player->getSheriffBadge());
		}
		if (player->isInTurn()) {
			m_window->draw(player->getTurnIndicator());
		}
	}

	// Buttons
	m_window->draw(m_ButtonLeft);
	m_window->draw(m_ButtonLeftText);
	m_window->draw(m_ButtonRight);
	m_window->draw(m_ButtonRightText);

	// A mask to focus on withdraw event, filter out avatars, buttons,...
	if (m_gameEvent == WITHDRAW) {
		m_window->draw(m_withdrawEventMask);
	}

	// User hand
	for (const auto& card : m_userHand) {
		m_window->draw(card->getCard());
	}

	// Decks
	m_window->draw(m_deck->getMainDeck());
	m_window->draw(m_deck->getDiscardDeckLeft());
	m_window->draw(m_deck->getDiscardDeckRight());

	// Selected cards, for discard,present...
	for (const auto& card : m_selectedCards) {
		m_window->draw(card->getCard());
	}

	// Render the tablet if shown
	if (m_tablet->isTabletVisible()) {
		m_tablet->render();
	}

	// Render the timer if running
	if (m_timer->isRunning()) {
		m_timer->render();
	}

	m_window->display();

	return true;
}

bool Game::setupPlayerUI()
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->isUserPlayer()) {
			// The buttons are colored only when it is user's turn
			m_ButtonLeft.setFillColor(m_playerList[i]->isInTurn() ? sf::Color::Red : sf::Color(128, 128, 128));
			m_ButtonRight.setFillColor(m_playerList[i]->isInTurn() ? sf::Color(100, 149, 237) : sf::Color(128, 128, 128));

			// Render button texts based on user player role
			m_ButtonLeftText.setString(m_playerList[i]->isSheriff() ? "Inspect" : "Discard");
			m_ButtonRightText.setString(m_playerList[i]->isSheriff() ? "Pass" : "Present");
			// Center the text within the button
			sf::FloatRect textBoundsLeft = m_ButtonLeftText.getLocalBounds();
			sf::FloatRect textBoundsRight = m_ButtonRightText.getLocalBounds();
			m_ButtonLeftText.setOrigin(textBoundsLeft.left + textBoundsLeft.width / 2, textBoundsLeft.top + textBoundsLeft.height / 2);
			m_ButtonRightText.setOrigin(textBoundsRight.left + textBoundsRight.width / 2, textBoundsRight.top + textBoundsRight.height / 2);

			continue;
		}
		// Positioning
		float xOffset = -230.f + (i % 6) * 380.f; // Distribute evenly horizontally

		m_playerList[i]->initPlayer(xOffset, 50.f);
	}

	return true;
}

void Game::userHandUI()
{
	m_anyCardSelected = false;
	for (int i = 0; i < m_userHand.size(); i++) {
		float posX = 520.f + (i % 6) * 150.f;
		if (m_userHand[i]->isSelected()) {
			//m_userHand[i]->setupCardUI(posX, 600.f);
			m_animationPlayer.addAnimation(new Animation(m_userHand[i]->getCard(), Animation::Type::MOVE, 0.3, sf::Vector2f(posX, 600.f), 1.f, 0.5));
			m_anyCardSelected = true;
		}
		else {
			//m_userHand[i]->setupCardUI(posX, 635.f);
			m_animationPlayer.addAnimation(new Animation(m_userHand[i]->getCard(), Animation::Type::MOVE, 0.3, sf::Vector2f(posX, 635.f), 1.f, 0.5));
		}
	}
}

void Game::updateUserHandAnimation(float deltaTime)
{
	for (auto& card : m_userHand) {
		card->getAnimationPlayer().update(deltaTime);
	}
}

void Game::onMessageReceived(const nlohmann::json& jsonMessage)
{
	// For testing
	std::cout << "Game receive from Server: " << jsonMessage << std::endl;

	// Game Deals Cards
	if (jsonMessage["MessageType"] == "GAME_DEALS_CARDS" && jsonMessage.contains("Cards")) {
		std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		int index = 0;
		// Get all Card types
		for (auto it = jsonMessage["Cards"].begin(); it != jsonMessage["Cards"].end(); it++) {
			if (m_userHand.size() >= 6U) break;	// Limit guard
			// Get card type as string
			std::string cardName = *it;

			// Add to user hand
			addToUserHand(Card::m_stringToCardName.at(cardName));

			// Add drawing animation of that card from the main deck
			m_userHand[m_userHand.size() - 1]->getCard().setPosition(m_deck->getMainDeck().getPosition() + sf::Vector2f(75, 100));
			m_userHand[m_userHand.size() - 1]->getCard().setScale(sf::Vector2f(0.f, 0.f));
			float posX = 520.f + ((m_userHand.size()-1) % 6) * 150.f;
			float delay = index * 0.3;
			// Add animation, after animation done, send response message
			if (m_userHand.size() == 6U) {
				m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
					0.1, sf::Vector2f(posX, 635.f), 1.f, delay, [this, jsonMessage] { Network::getInstance().respondMessage(jsonMessage); }));
			}
			else {
				m_userHand[m_userHand.size() - 1]->animationMove(0.1, sf::Vector2f(posX, 635), 1.f, delay);
			}
			index++;
		}
	}

	// Start a new turn
	if (jsonMessage["MessageType"] == "GAME_START_TURN") {
		std::string playerName = jsonMessage["PlayerName"];
		for (auto& player : m_playerList) {
			if (player->getPlayerName() == playerName) {
				player->setTurn(true);

				// Start the timer if user player
				if (player->isUserPlayer()) {
					m_timer->reset();
					m_timer->start();
				}
			}
		}
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server confirm GiveBag event of user
	if ( jsonMessage["MessageType"] == "MERCHANT_GIVE_BAG_RESPONSE" && 
		jsonMessage["PlayerName"] == m_playerList[0]->getPlayerName() ) 
	{
		if (!m_userHand.empty()) {
			// Move presented cards from user hand to present bag
			for (int i = 0; i < m_userHand.size(); i++) {
				if (m_userHand[i]->isSelected()) {
					m_selectedCards.push_back(m_userHand[i]);
					removeFromUserHand(i);
					i--;
				}
			}
			for (auto& card : m_selectedCards) {
				m_animationPlayer.addAnimation(new Animation(card->getCard(), Animation::Type::SCALE, 0.f, 0.3));
			}
			// Re-arrange userhand
			userHandUI();
		}
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server confirm player withdrawing
	if (jsonMessage["MessageType"] == "MERCHANT_WITHDRAW_CARDS_RESPONSE") {
		// Check if it is user's withdraw
		if (jsonMessage["PlayerName"] == m_playerList[0]->getPlayerName()) {
			if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
				Card* topCard = m_deck->getStackLeft().top();

				// Add new card to user hand and start animation
				addToUserHand(topCard->getCardType());
				m_userHand[m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
				m_userHand[m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(650.f, 324.f));
				float posX = 520.f + ((m_userHand.size() - 1) % 6) * 150.f;
				m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
					0.2, sf::Vector2f(posX, 635.f)));

				// Pop the left deck
				m_deck->getStackLeft().pop();
				delete topCard;
				m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());

				// If done withdrawing, change game state
				if (m_userHand.size() == 6U) {
					m_gameEvent = DEFAULT;
				}

				// Send a response message
				Network::getInstance().respondMessage(jsonMessage);
			}
			else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
				Card* topCard = m_deck->getStackRight().top();

				// Add new card to user hand and start animation
				addToUserHand(topCard->getCardType());
				m_userHand[m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
				m_userHand[m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(1162.f, 324.f));
				float posX = 520.f + ((m_userHand.size() - 1) % 6) * 150.f;
				m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
					0.2, sf::Vector2f(posX, 635.f)));

				// Pop the right deck
				m_deck->getStackRight().pop();
				delete topCard;
				m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());

				// If done withdrawing, change game state
				if (m_userHand.size() == 6U) {
					m_gameEvent = DEFAULT;
				}

				// Send a response message
				Network::getInstance().respondMessage(jsonMessage);
			}
			else if (jsonMessage["Pile"] == "MAIN_DECK") {
				// Add new card to user hand and start animation
				addToUserHand(Card::m_stringToCardName.at(jsonMessage["Card"]));
				m_userHand[m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
				m_userHand[m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(885.f, 280.f));
				float posX = 520.f + ((m_userHand.size() - 1) % 6) * 150.f;
				m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
					0.2, sf::Vector2f(posX, 635.f)));

				// If done withdrawing, change game state
				if (m_userHand.size() == 6U) {
					m_gameEvent = DEFAULT;
				}

				// Send a response message
				Network::getInstance().respondMessage(jsonMessage);
			}
		}
	}


}




