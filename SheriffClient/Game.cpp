#include "game.h"
#include <Windows.h>

void Game::initVariables(std::vector<Player*> playerList)
{
	m_gameEvent = Game::DEFAULT;
	m_anyCardSelected = false;
	m_anyCardDragged = false;
	m_window = nullptr;
	if (!m_backgroundTexture.loadFromFile("Images/Background.png")) {
		std::cerr << "Error loading background texture!";
	}
	m_backgroundTexture.setSmooth(true);

	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	m_dragOffset = sf::Vector2f(0.f, 0.f);
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
	m_deck->getStackLeft().push(new Card(Card::SILK));
	m_deck->getStackLeft().push(new Card(Card::PEPPER));
	m_deck->getStackLeft().push(new Card(Card::CHICKEN));
	m_deck->getStackLeft().push(new Card(Card::APPLE));
	m_deck->getStackLeft().push(new Card(Card::MEAD));
	m_deck->getStackLeft().push(new Card(Card::BREAD));
	m_deck->getStackRight().push(new Card(Card::APPLE));
	m_deck->getStackRight().push(new Card(Card::APPLE));
	m_deck->getStackRight().push(new Card(Card::BREAD));
	m_deck->getStackRight().push(new Card(Card::CROSSBOW));
	m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());
	m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());
	// End of Test

	// Backgrounds
	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setFillColor(sf::Color::White);
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));
	m_background.setTexture(&m_backgroundTexture);
	m_withdrawEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_withdrawEventMask.setFillColor(sf::Color(0, 0, 0, 180));
	m_discardEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_discardEventMask.setFillColor(sf::Color(0, 0, 0, 180));

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

	// Reset tablet and wait for server response
	m_tablet->resetOptions();
	m_gameEvent = Game::IDLE;
}

void Game::handleWithdrawEvent(PileType type)
{
	Card* topCard = nullptr;
	float posX = 0.f;
	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		if (m_deck->getStackLeft().empty()) return;			// Prevent drawing from null
		
		topCard = m_deck->getStackLeft().top();
		// Add new card to user hand and start animation
		addToUserHand(topCard->getCardType());
		m_userHand[m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
		m_userHand[m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(650.f, 324.f));
		posX = 520.f + ((m_userHand.size() - 1) % 6) * 150.f;
		m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.2, sf::Vector2f(posX, 635.f)));

		// Pop the left deck
		m_deck->getStackLeft().pop();
		delete topCard;
		if (!m_deck->getStackLeft().empty()) {
			m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());
		}
		else {
			m_deck->setDiscardDeckLeftOutOfStockTexture();
		}

		break;
	case Game::PileType::RIGHT_DISCARD_PILE:
		if (m_deck->getStackRight().empty()) return;			// Prevent drawing from null

		topCard = m_deck->getStackRight().top();
		// Add new card to user hand and start animation
		addToUserHand(topCard->getCardType());
		m_userHand[m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
		m_userHand[m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(1162.f, 324.f));
		posX = 520.f + ((m_userHand.size() - 1) % 6) * 150.f;
		m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.2, sf::Vector2f(posX, 635.f)));

		// Pop the right deck
		m_deck->getStackRight().pop();
		delete topCard;
		if (!m_deck->getStackRight().empty()) {
			m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());
		}
		else {
			m_deck->setDiscardDeckRightOutOfStockTexture();
		}

		break;
	case Game::PileType::MAIN_DECK:
		m_userHand[m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
		m_userHand[m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(885.f, 280.f));
		posX = 520.f + ((m_userHand.size() - 1) % 6) * 150.f;
		m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.2, sf::Vector2f(posX, 635.f)));

		break;
	default:
		break;
	}
}

void Game::handleDiscardEvent(PileType type, std::string cardName)
{
	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		for (int i = 0; i < m_selectedCards.size(); i++) {
			// Look for the right card to insert to pile
			if (m_selectedCards[i]->isDragging() && m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName)) {
				m_deck->getStackLeft().push(m_selectedCards[i]);
				// Animation card fit into deck, callback to function set texture
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::MOVE,
					0.2, sf::Vector2f(650.f, 324.f), 0.8, 0.f, [this]
				{this->getDeck()->setDiscardDeckLeftTexture(this->m_deck->getStackLeft().top()->getCardType());}));
				// Animation card fit into deck, callback to remove from selected cards
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::SCALE,
					0.8, 0.2, 0.f, [this, i] {this->getSelectedCards().erase(this->getSelectedCards().begin() + i); this->getGameEvent() = Game::DISCARD; }));

				// Reset status
				m_anyCardDragged = false;

				break;
			}
		}

		break;

	case Game::PileType::RIGHT_DISCARD_PILE:
		for (int i = 0; i < m_selectedCards.size(); i++) {
			// Look for the right card to insert to pile
			if (m_selectedCards[i]->isDragging() && m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName)) {
				m_deck->getStackRight().push(m_selectedCards[i]);
				// Animation card fit into deck, callback to function set texture
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::MOVE,
					0.2, sf::Vector2f(1162.f, 324.f), 0.8, 0.f, [this]
				{this->getDeck()->setDiscardDeckRightTexture(this->m_deck->getStackRight().top()->getCardType()); }));
				// Animation card fit into deck, callback to remove from selected cards
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::SCALE,
					0.8, 0.2, 0.f, [this, i] {this->getSelectedCards().erase(this->getSelectedCards().begin() + i); this->getGameEvent() = Game::DISCARD; }));

				// Reset status
				m_anyCardDragged = false;

				break;
			}
		}

		break;

	default:
		break;
	}
}

void Game::handleOpponentWithdrawEvent(PileType type, int playerIndex)
{
	Card* dummyCard = nullptr;
	sf::Vector2f endPos = m_playerList[playerIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f);
	int index = 0;
	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		if (m_deck->getStackLeft().empty()) return;			// Prevent drawing from null

		m_dummyCards.push_back(m_deck->getStackLeft().top());
		index = m_dummyCards.size() - 1;
		dummyCard = m_dummyCards[index];
		dummyCard->getCard().setPosition(650.f, 324.f);
		// Add animation withdraw for the player, pop the left deck and set new texture once animation is done
		m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 1.f,
			endPos, 0.3, 0.f, [this, dummyCard, index]
		{ 
			delete dummyCard;
			if (!m_dummyCards.empty()) { m_dummyCards.erase(m_dummyCards.begin()); }
		}));

		m_deck->getStackLeft().pop();
		if (!m_deck->getStackLeft().empty()) {
			m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());
		}
		else {
			m_deck->setDiscardDeckLeftOutOfStockTexture();
		}

		break;
	case Game::PileType::RIGHT_DISCARD_PILE:
		if (m_deck->getStackRight().empty()) return;			// Prevent drawing from null

		m_dummyCards.push_back(m_deck->getStackRight().top());
		index = m_dummyCards.size() - 1;
		dummyCard = m_dummyCards[index];
		dummyCard->getCard().setPosition(1162.f, 324.f);
		// Add animation withdraw for the player, pop the left deck and set new texture once animation is done
		m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 1.f,
			endPos, 0.3, 0.f, [this, dummyCard, index]
		{
			delete dummyCard;
			if (!m_dummyCards.empty()) { m_dummyCards.erase(m_dummyCards.begin()); }
		}));

		m_deck->getStackRight().pop();
		if (!m_deck->getStackRight().empty()) {
			m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());
		}
		else {
			m_deck->setDiscardDeckRightOutOfStockTexture();
		}

		break;
	case Game::PileType::MAIN_DECK:
		// Add animation withdraw of an unknown card
		m_dummyCards.push_back(new Card(Card::UNKNOWN));
		index = m_dummyCards.size() - 1;
		dummyCard = m_dummyCards[index];
		dummyCard->getCard().setPosition(885.f, 280.f);
		m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 1.f,
			endPos, 0.3, 0.f, [this, dummyCard, index]
		{
			delete dummyCard;
			if (!m_dummyCards.empty()) { m_dummyCards.erase(m_dummyCards.begin()); }
		}));

		break;

	default:
		break;
	}
}

void Game::handleOpponentDiscardEvent(PileType type, int playerIndex, Card::CardType cardType)
{
	sf::Vector2f startPos = m_playerList[playerIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f);
	sf::Vector2f startScale = sf::Vector2f(0.3, 0.3);
	sf::Vector2f endPosLeft = sf::Vector2f(650.f, 324.f);
	sf::Vector2f endPosRight = sf::Vector2f(1162.f, 324.f);
	Card* dummyCard = nullptr;
	int index = 0;

	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		m_deck->getStackLeft().push(new Card(cardType));
		m_dummyCards.push_back(m_deck->getStackLeft().top());
		index = m_dummyCards.size() - 1;
		dummyCard = m_dummyCards[index];
		dummyCard->getCard().setPosition(startPos);
		dummyCard->getCard().setScale(startScale);
		// Add animation discard card
		m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 1.f, 
			endPosLeft, 0.8, 0.f, [this] 
		{
			m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());
			if (!m_dummyCards.empty()) { m_dummyCards.erase(m_dummyCards.begin()); }
		}));

		break;
	case Game::PileType::RIGHT_DISCARD_PILE:
		m_deck->getStackRight().push(new Card(cardType));
		m_dummyCards.push_back(m_deck->getStackRight().top());
		index = m_dummyCards.size() - 1;
		dummyCard = m_dummyCards[index];
		dummyCard->getCard().setPosition(startPos);
		dummyCard->getCard().setScale(startScale);
		// Add animation discard card
		m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 1.f,
			endPosRight, 0.8, 0.f, [this]
		{
			m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());
			if (!m_dummyCards.empty()) { m_dummyCards.erase(m_dummyCards.begin()); }
		}));

		break;

	default:
		break;
	}
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

std::vector<Card*>& Game::getSelectedCards()
{
	return m_selectedCards;
}

Deck * Game::getDeck()
{
	return m_deck;
}

Game::GameEvent & Game::getGameEvent()
{
	return m_gameEvent;
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
		// Only interactable if that deck has any card left
		if (m_deck->getStackLeft().empty()) return false;

		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[0]->getPlayerName();
		message["Pile"] = "LEFT_DISCARD_DECK";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
	}

	// Right deck is clicked when withdrawing
	if (m_deck->getDiscardDeckRight().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		// Only interactable if that deck has any card left
		if (m_deck->getStackRight().empty()) return false;

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

	// A selected card is clicked when discarding
	for (auto& card : m_selectedCards) {
		if (card->getCard().getGlobalBounds().contains(mousePosXY) && m_gameEvent == DISCARD) {
			// Start the drag and save static position
			if (!card->isDragging() && !m_anyCardDragged) {
				card->setupCardUI(card->getCard().getPosition().x, card->getCard().getPosition().y, sf::Vector2f(0.72, 0.72));
				card->getCard().setScale(1, 1);
				m_dragOffset = card->getCard().getPosition() - mousePosXY;
				m_anyCardDragged = true;
				card->setDragging(true);
			}
		}
	}

	return true;
}

bool Game::handleMouseDrag(sf::Vector2f mousePosXY)
{
	for (auto& card : m_selectedCards) {
		if (card->isDragging() && m_gameEvent == DISCARD) {
			card->getCard().setPosition(mousePosXY + m_dragOffset);
		}
	}

	return true;
}

bool Game::handleMouseRelease()
{
	for (int i = 0; i < m_selectedCards.size(); i++) {
		if (m_selectedCards[i]->isDragging()) {
			// If the dragged card is placed on top of left discard pile
			if (m_selectedCards[i]->getCard().getGlobalBounds().intersects(m_deck->getDiscardDeckLeft().getGlobalBounds())) {
				// Notify server
				json message;
				message["MessageType"] = "MERCHANT_DISCARD_CARDS";
				message["PlayerName"] = m_playerList[0]->getPlayerName();
				message["Pile"] = "LEFT_DISCARD_PILE";
				message["Card"] = Card::m_cardNameToString.at(m_selectedCards[i]->getCardType());
				std::string messageString = message.dump();
				Network::getInstance().sendMessage(messageString);

				// Wait for server confirmation
				m_gameEvent = IDLE;
			}

			// If the dragged card is placed on top of right discard pile
			else if (m_selectedCards[i]->getCard().getGlobalBounds().intersects(m_deck->getDiscardDeckRight().getGlobalBounds())) {
				// Notify server
				json message;
				message["MessageType"] = "MERCHANT_DISCARD_CARDS";
				message["PlayerName"] = m_playerList[0]->getPlayerName();
				message["Pile"] = "RIGHT_DISCARD_PILE";
				message["Card"] = Card::m_cardNameToString.at(m_selectedCards[i]->getCardType());
				std::string messageString = message.dump();
				Network::getInstance().sendMessage(messageString);

				// Wait for server confirmation
				m_gameEvent = IDLE;
			}

			else {
				// If the dragged card is not placed on anything, reset its UI
				m_selectedCards[i]->setDragging(false);
				m_anyCardDragged = false;
				m_selectedCards[i]->resetStaticPosition();
			}
		}
	}

	return true;
}

bool Game::pollEvents()
{
	while (m_window->pollEvent(m_ev)) {
		sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_window);
		sf::Vector2f mousePosXY = m_window->mapPixelToCoords(mousePosXYLocal);
		switch (m_ev.type) {
		case sf::Event::Closed:
			m_window->close();
			break;

		case sf::Event::MouseButtonPressed:
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				handleMouseClick(mousePosXY);
			}
			break;

		case sf::Event::MouseMoved:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				handleMouseDrag(mousePosXY);
			}
			break;

		case sf::Event::MouseButtonReleased:
			handleMouseRelease();

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
	case Game::IDLE:
		// Do nothing
		break;

	case Game::DEFAULT:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		setupPlayerUI();

		// If tablet is shown, it is interactable
		m_tablet->update();

		break;

	case Game::DISCARD:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		setupPlayerUI();

		// If all cards are discarded, change game state
		if (m_selectedCards.empty()) {
			m_gameEvent = DEFAULT;
		}

		break;

	case Game::WITHDRAW:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		setupPlayerUI();

		// If done withdrawing, change game state
		if (m_userHand.size() == 6U) {
			m_gameEvent = DISCARD;
		}

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
	// A mask to focus on discard event, filter out the main deck
	if (m_gameEvent == DISCARD || (m_gameEvent == IDLE && !m_tablet->isTabletVisible())) {
		m_window->draw(m_discardEventMask);
	}
	m_window->draw(m_deck->getDiscardDeckLeft());
	m_window->draw(m_deck->getDiscardDeckRight());
	// Draw opponent's withdraw / discard animation if needed
	//if (m_dummyCard != nullptr) {
	//	m_window->draw(m_dummyCard->getCard());
	//}
	for (auto& card : m_dummyCards) {
		m_window->draw(card->getCard());
	}

	// Selected cards, for discard,present...
	Card* draggedCard = nullptr;
	for (const auto& card : m_selectedCards) {
		if (!card->isDragging())
			m_window->draw(card->getCard());
		else
			draggedCard = card;
	}
	// The dragged card is rendered last
	if (draggedCard) {
		m_window->draw(draggedCard->getCard());
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
		std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
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
			// Hide tablet
			m_tablet->hideTablet();
			// Re-arrange userhand
			userHandUI();
			m_gameEvent = DEFAULT;
		}
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server confirm player withdrawing
	if (jsonMessage["MessageType"] == "MERCHANT_WITHDRAW_CARDS_RESPONSE") {
		std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		// Check if it is user's withdraw
		if (jsonMessage["PlayerName"] == m_playerList[0]->getPlayerName()) {
			if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
				handleWithdrawEvent(Game::PileType::LEFT_DISCARD_PILE);
			}
			else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
				handleWithdrawEvent(Game::PileType::RIGHT_DISCARD_PILE);
			}
			else if (jsonMessage["Pile"] == "MAIN_DECK") {
				// Add new card to user hand and start animation
				addToUserHand(Card::m_stringToCardName.at(jsonMessage["Card"]));
				handleWithdrawEvent(Game::PileType::MAIN_DECK);
			}
		}
		
		// If it is not user's withdraw, setup animation of opponent's withdraw
		else {
			for (int i = 1; i < m_playerList.size(); i++) {
				if (jsonMessage["PlayerName"] == m_playerList[i]->getPlayerName()) {
					if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
						handleOpponentWithdrawEvent(Game::PileType::LEFT_DISCARD_PILE, i);
					}
					else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
						handleOpponentWithdrawEvent(Game::PileType::RIGHT_DISCARD_PILE, i);
					}
					else if (jsonMessage["Pile"] == "MAIN_DECK") {
						handleOpponentWithdrawEvent(Game::PileType::MAIN_DECK, i);
					}
				}
			}
		}

		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server confirm player discarding
	if (jsonMessage["MessageType"] == "MERCHANT_DISCARD_CARDS_RESPONSE") {
		std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		// Check if it is user's discard
		if (jsonMessage["PlayerName"] == m_playerList[0]->getPlayerName()) {
			std::string cardName = jsonMessage["Card"];
			if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
				handleDiscardEvent(Game::PileType::LEFT_DISCARD_PILE, cardName);
			}
			else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
				handleDiscardEvent(Game::PileType::RIGHT_DISCARD_PILE, cardName);
			}
		}

		// If it is not user's discard, setup animation of opponent's discard
		else {
			for (int i = 1; i < m_playerList.size(); i++) {
				if (jsonMessage["PlayerName"] == m_playerList[i]->getPlayerName()) {
					// Get card type from message
					Card::CardType card = Card::m_stringToCardName.at(jsonMessage["Card"]);
					if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
						handleOpponentDiscardEvent(Game::PileType::LEFT_DISCARD_PILE, i, card);
					}
					else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
						handleOpponentDiscardEvent(Game::PileType::RIGHT_DISCARD_PILE, i, card);
					}
				}
			}
		}

		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

}




