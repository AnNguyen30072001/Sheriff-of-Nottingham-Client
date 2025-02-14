#include "game.h"
#include <Windows.h>
#include <string.h>
#include <random>
#include <ctime>

void Game::initVariables(std::vector<Player*> playerList)
{
	if (!m_backgroundTexture.loadFromFile("Images/Background.png")) {
		std::cerr << "Error loading background texture!";
	}
	m_backgroundTexture.setSmooth(true);

	if (!m_moneyIconTexture.loadFromFile("Images/MoneyIcon.png")) {
		std::cerr << "Error loading game texture!";
	}

	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	if (!m_glowShader.loadFromFile("Shaders/glow.frag", sf::Shader::Fragment)) {
		std::cerr << "Error loading shader!\n";
	}

	m_gameEvent = Game::DEFAULT;
	m_lastUpdatedGameEvent = DEFAULT;
	m_elapsedTime = 0.f;
	m_anyCardSelected = false;
	m_anyCardDragged = false;
	m_discardDone = false;
	m_revealingDone = false;
	m_window = nullptr;
	m_bribeAmount = 0;
	m_goodsReport = Card::UNKNOWN;
	m_dragOffset = sf::Vector2f(0.f, 0.f);
	m_playerList = playerList;
	m_deck = nullptr;
	m_tablet = nullptr;
	m_timer = nullptr;
	m_disconnectPopup = nullptr;
	m_MerchantShowingBagIndex = 0;
}

void Game::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	for (int i = 1; i < m_playerList.size(); i++) {
		// Positioning players
		float xOffset = -230.f + (i % 6) * 380.f; // Distribute evenly horizontally
		m_playerList[i]->initPlayer(xOffset, 50.f);
	}

	m_deck = new Deck();
	m_tablet = new Tablet(m_window);
	m_timer = new Timer(m_window);
	m_disconnectPopup = new Popup(m_window, 400, 200);

	// For testing only
	//m_deck->getStackLeft().push(new Card(Card::SILK));
	//m_deck->getStackLeft().push(new Card(Card::PEPPER));
	//m_deck->getStackLeft().push(new Card(Card::CHICKEN));
	//m_deck->getStackLeft().push(new Card(Card::APPLE));
	//m_deck->getStackLeft().push(new Card(Card::MEAD));
	//m_deck->getStackLeft().push(new Card(Card::BREAD));
	//m_deck->getStackRight().push(new Card(Card::APPLE));
	//m_deck->getStackRight().push(new Card(Card::APPLE));
	//m_deck->getStackRight().push(new Card(Card::BREAD));
	//m_deck->getStackRight().push(new Card(Card::CROSSBOW));
	//m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());
	//m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());
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
	m_SheriffEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_SheriffEventMask.setFillColor(sf::Color(0, 0, 0, 180));

	// Init shaders
	m_glowShader.setUniform("isHovered", false);
	m_glowShader.setUniform("outlineColor", sf::Glsl::Vec4(1.0, 1.0, 1.0, 1.0));
	m_glowShader.setUniform("glowStrength", 0.35f); // Adjust glow thickness

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

	// Setup bribe amount, goods report
	m_goodsReportText.setFont(m_font);
	m_goodsReportText.setCharacterSize(48);
	m_goodsReportText.setFillColor(sf::Color::White);
	m_moneyText.setFont(m_font);
	m_moneyText.setCharacterSize(48);
	m_moneyText.setFillColor(sf::Color::White);
	m_moneyIcon.setSize(sf::Vector2f(100.f, 100.f));
	m_moneyIcon.setFillColor(sf::Color::White);
	m_moneyIcon.setTexture(&m_moneyIconTexture);

	// Info text
	m_infoText.setFont(m_font);
	m_infoText.setCharacterSize(48);
	m_infoText.setFillColor(sf::Color::White);

	// Guide text
	m_guideText.setFont(m_font);
	m_guideText.setCharacterSize(40);
	m_guideText.setFillColor(sf::Color::White);

	// Positioning buttons
	m_ButtonLeft.setPosition(500.f, 905.f);
	m_ButtonRight.setPosition(1245.f, 905.f);
	m_ButtonLeftText.setPosition(m_ButtonLeft.getPosition().x + m_ButtonLeft.getSize().x / 2,
		m_ButtonLeft.getPosition().y + m_ButtonLeft.getSize().y / 2);
	m_ButtonRightText.setPosition(m_ButtonRight.getPosition().x + m_ButtonRight.getSize().x / 2,
		m_ButtonRight.getPosition().y + m_ButtonRight.getSize().y / 2);

	// Positioning bribe amount, goods report
	m_goodsReportText.setPosition(804.f, 215.f);
	m_moneyIcon.setPosition(910.f, 515.f);
	m_moneyText.setPosition(1030.f, 534.f);

	// Initial draw user hand
	userHandUI();
}

bool Game::attemptReconnect(float deltaTime)
{
	m_elapsedTime += deltaTime;
	// Attempt reconnect every 5 seconds
	if (m_elapsedTime < 5.f) {
		return false;
	}
	// Attempt connection to server
	if (!Network::getInstance().connect()) {
		m_elapsedTime = 0.f;
		return false;
	}

	// Connection success.
	else {
		Network::getInstance().startListening();
		Network::getInstance().startProcessingMessageQueue();
		return true;
	}
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

	// Save information for next Sheriff turn
	m_goodsReport = m_tablet->getPresentedGoods();
	m_bribeAmount = m_tablet->getBribeAmount();

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
	// Only discard once per turn
	m_discardDone = true;

	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		for (int i = 0; i < m_selectedCards.size(); i++) {
			// Look for the right card to insert to pile
			if ((m_selectedCards[i]->isDragging() && m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName)) 
				|| (m_gameEvent == TIMEOUT_DISCARD && m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName))) {
				m_deck->getStackLeft().push(m_selectedCards[i]);
				// Animation card fit into deck, callback to function set texture
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::MOVE,
					0.2, sf::Vector2f(650.f, 324.f), 0.8, 0.f, [this]
				{this->getDeck()->setDiscardDeckLeftTexture(this->m_deck->getStackLeft().top()->getCardType());}));
				// Animation card fit into deck, callback to remove from selected cards
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::SCALE,
					0.8, 0.2, 0.f, [this, i] 
				{
					this->getSelectedCards().erase(this->getSelectedCards().begin() + i); 
					if (m_gameEvent == TIMEOUT_DISCARD) m_gameEvent = TIMEOUT_DISCARD;
					else m_gameEvent = Game::DISCARD;
				}));

				// Reset status
				m_anyCardDragged = false;

				break;
			}
		}

		break;

	case Game::PileType::RIGHT_DISCARD_PILE:
		for (int i = 0; i < m_selectedCards.size(); i++) {
			// Look for the right card to insert to pile
			if ((m_selectedCards[i]->isDragging() && m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName)) 
				|| (m_gameEvent == TIMEOUT_DISCARD && m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName))) {
				m_deck->getStackRight().push(m_selectedCards[i]);
				// Animation card fit into deck, callback to function set texture
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::MOVE,
					0.2, sf::Vector2f(1162.f, 324.f), 0.8, 0.f, [this]
				{this->getDeck()->setDiscardDeckRightTexture(this->m_deck->getStackRight().top()->getCardType()); }));
				// Animation card fit into deck, callback to remove from selected cards
				m_animationPlayer.addAnimation(new Animation(m_selectedCards[i]->getCard(), Animation::Type::SCALE,
					0.8, 0.2, 0.f, [this, i] 
				{
					this->getSelectedCards().erase(this->getSelectedCards().begin() + i); 
					if (m_gameEvent == TIMEOUT_DISCARD) m_gameEvent = TIMEOUT_DISCARD;
					else m_gameEvent = Game::DISCARD;
				}));

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

void Game::handleSheriffInspectEvent(const nlohmann::json & jsonMessage)
{
	// Setup
	int revealedIndex = 0;
	std::vector<Card::CardType> cardTypes;
	for (auto it = jsonMessage["Bag"].begin(); it != jsonMessage["Bag"].end(); it++) {
		std::string cardName = *it;
		cardTypes.push_back(Card::m_stringToCardName.at(cardName));
	}

	Player* sheriffPlayer = nullptr;
	for (auto& player : m_playerList) {
		if (player->getPlayerName() == jsonMessage["PlayerName"]) {
			sheriffPlayer = player;
		}
	}
	// Add animation Merchant retrieve bribe
	m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.5,
		m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f));
	m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.5, 
		m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f, [this, sheriffPlayer, cardTypes, revealedIndex, jsonMessage]
	{
		m_moneyText.setCharacterSize(30);
		// Recursively reveal cards, until all cards are revealed
		revealCard(sheriffPlayer, cardTypes, revealedIndex, jsonMessage);
	}));
}

void Game::handleSheriffPassEvent(const nlohmann::json & jsonMessage)
{
	// Setup
	int revealedIndex = 0;
	std::vector<Card::CardType> cardTypes;
	for (auto it = jsonMessage["Bag"].begin(); it != jsonMessage["Bag"].end(); it++) {
		std::string cardName = *it;
		cardTypes.push_back(Card::m_stringToCardName.at(cardName));
	}

	Player* sheriffPlayer = nullptr;
	for (auto& player : m_playerList) {
		if (player->getPlayerName() == jsonMessage["PlayerName"]) {
			sheriffPlayer = player;
		}
	}

	// Add animation Sheriff receive bribe
	m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.5,
		sheriffPlayer->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f));
	m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.5,
		sheriffPlayer->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f, [this, sheriffPlayer, cardTypes, revealedIndex, jsonMessage]
	{
		m_moneyText.setCharacterSize(30);
		// Update sheriff and merchant catalog
		m_playerList[m_MerchantShowingBagIndex]->setPlayerMoney(m_playerList[m_MerchantShowingBagIndex]->getPlayerMoney() - m_bribeAmount);
		sheriffPlayer->setPlayerMoney(sheriffPlayer->getPlayerMoney() + m_bribeAmount);
		// Recursively reveal cards, until all cards are revealed
		revealCard(sheriffPlayer, cardTypes, revealedIndex, jsonMessage);
	}));
}

void Game::revealCard(Player * sheriff, std::vector<Card::CardType> cardTypes, int revealIndex, const nlohmann::json& jsonMessage)
{
	// Add animation to reveal a card
	m_animationPlayer.addAnimation(new Animation(m_dummyCards[revealIndex]->getCard(), Animation::Type::SCALE, 1.2, 0.2, 0.2, [this, cardTypes, sheriff, revealIndex, jsonMessage]
	{
		// Reveal the card
		m_dummyCards[revealIndex]->setCardTexture(cardTypes[revealIndex]);
		m_animationPlayer.addAnimation(new Animation(m_dummyCards[revealIndex]->getCard(), Animation::Type::SCALE, 1.0, 0.2, 0.f, [this, cardTypes, sheriff, revealIndex, jsonMessage]
		{
			sf::Vector2f posOffset = m_dummyCards[revealIndex]->getCard().getPosition();

			// If the card is legal or sheriff chose to Pass, merchant get the cash
			if (cardTypes[revealIndex] == m_goodsReport || jsonMessage["MessageType"] == "SHERIFF_PASS_RESPONSE") {
				std::cout << "Legal\n";
				int depositCash = Card::cardTypeToValue.at(cardTypes[revealIndex]);
				m_moneyText.setString(std::to_string(depositCash));
				m_moneyText.setScale(1.f, 1.f);
				m_moneyText.setFillColor(sf::Color::Green);
				m_moneyIcon.setScale(0.6, 0.6);
				m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
				m_moneyText.setPosition(m_moneyIcon.getPosition() + sf::Vector2f(20.f, 60.f));
				// Add animation Merchant receive the cash
				m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE, 0.15, 
					m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 1.f));
				m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE, 0.15,
					m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 1.f));
				m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.3,
					m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.5));
				m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
					m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.5, [this, cardTypes, sheriff, revealIndex, jsonMessage]
				{
					// Update player's catalog
					m_playerList[m_MerchantShowingBagIndex]->increasePlayerGoodsAmount(cardTypes[revealIndex], 1);
					m_playerList[m_MerchantShowingBagIndex]->setPlayerMoney(m_playerList[m_MerchantShowingBagIndex]->getPlayerMoney() 
						+ Card::cardTypeToValue.at(cardTypes[revealIndex]));
					
					// Continue revealing until all cards are revealed
					int nextIndex = revealIndex + 1;
					if (nextIndex < cardTypes.size()) {
						revealCard(sheriff, cardTypes, nextIndex, jsonMessage);
					}
					else {
						// Retrieve the cards laid on table
						retrieveCards(jsonMessage);
					}
				}));
			}
			// If the card is not legal, sheriff get the cash
			else {
				std::cout << "Not legal\n";
				// Add animation Sheriff receive the cash
				int depositCash = Card::cardTypeToPenalty.at(cardTypes[revealIndex]);
				m_moneyText.setString(std::to_string(depositCash));
				m_moneyText.setScale(1.f, 1.f);
				m_moneyText.setFillColor(sf::Color::Green);
				m_moneyIcon.setScale(0.6, 0.6);
				m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
				m_moneyText.setPosition(m_moneyIcon.getPosition() + sf::Vector2f(20.f, 60.f));

				m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE, 0.15,
					m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 1.f));
				m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE, 0.15,
					m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 1.f));
				m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.2,
					sheriff->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.5));
				m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
					sheriff->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.5, [this, cardTypes, sheriff, revealIndex, jsonMessage, posOffset]
				{
					// Update player's catalog
					sheriff->setPlayerMoney(sheriff->getPlayerMoney()
						+ Card::cardTypeToPenalty.at(cardTypes[revealIndex]));

					// Add animation Merchant got deducted
					int penalty = Card::cardTypeToPenalty.at(cardTypes[revealIndex]);
					m_moneyText.setString("-" + std::to_string(penalty));
					m_moneyText.setScale(1.f, 1.f);
					m_moneyText.setFillColor(sf::Color::Red);
					m_moneyIcon.setScale(0.6, 0.6);
					m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
					m_moneyText.setPosition(m_moneyIcon.getPosition() + sf::Vector2f(15.f, 60.f));

					m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE, 0.15,
						m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 1.f));
					m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE, 0.15,
						m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 1.f));
					m_animationPlayer.addAnimation(new Animation(m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.3,
						m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.5));
					m_animationPlayer.addAnimation(new Animation(m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
						m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.5, [this, cardTypes, sheriff, revealIndex, jsonMessage]
					{
						// Update player's catalog
						m_playerList[m_MerchantShowingBagIndex]->setPlayerMoney(m_playerList[m_MerchantShowingBagIndex]->getPlayerMoney()
							- Card::cardTypeToPenalty.at(cardTypes[revealIndex]));

						// Continue revealing until all cards are revealed
						int nextIndex = revealIndex + 1;
						if (nextIndex < cardTypes.size()) {
							revealCard(sheriff, cardTypes, nextIndex, jsonMessage);
						}
						else {
							// Retrieve the cards laid on table
							retrieveCards(jsonMessage);
						}
					}));
				}));
			}
		}));
	}));
}

void Game::retrieveCards(const nlohmann::json& jsonMessage)
{
	if (m_dummyCards.empty()) return;

	for (int i = 0; i < m_dummyCards.size(); i++) {
		// If card is legal, hand it to merchant
		if (m_dummyCards[i]->getCardType() == m_goodsReport || jsonMessage["MessageType"] == "SHERIFF_PASS_RESPONSE") {
			m_animationPlayer.addAnimation(new Animation(m_dummyCards[i]->getCard(), Animation::Type::MOVE_AND_SCALE, 0.4,
				m_playerList[m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.6, [this, i, jsonMessage]
			{
				// If done process the last card
				if (i == m_dummyCards.size() - 1) {
					// Reveal process is complete
					m_revealingDone = true;

					for (Card* card : m_dummyCards) {
						delete card;
						card = nullptr;
					}
					m_dummyCards.clear();

					// Send response message
					Network::getInstance().respondMessage(jsonMessage);
				}
			}));
		}
		// If card is not legal, shuffle it back to main deck
		else {
			m_animationPlayer.addAnimation(new Animation(m_dummyCards[i]->getCard(), Animation::Type::SCALE, 0.f, 0.3, 0.6, [this, i, jsonMessage]
			{
				// If done process the last card
				if (i == m_dummyCards.size() - 1) {
					// Reveal process is complete
					m_revealingDone = true;

					for (Card* card : m_dummyCards) {
						delete card;
						card = nullptr;
					}
					m_dummyCards.clear();

					// Send response message
					Network::getInstance().respondMessage(jsonMessage);
				}
			}));
		}
	}
}

void Game::handleTimeoutWithdraw()
{
	m_gameEvent = TIMEOUT_WITHDRAW;
	//int leftPileCardsNumber = m_deck->getStackLeft().size();
	//int rightPileCardsNumber = m_deck->getStackRight().size();
	//int leftDrawCount = 0;
	//int rightDrawCount = 0;
	//int middleDrawCount = 0;
	int cardsToDraw = 6 - m_userHand.size();
	for (int i = 0; i < cardsToDraw; i++) {
		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS_TIMEOUT";
		message["PlayerName"] = m_playerList[0]->getPlayerName();
		message["Pile"] = "MAIN_DECK";
		std::string messageString = message.dump();
		Network::getInstance().sendMessage(messageString);
	}
}

void Game::handleTimeoutDiscard()
{
	m_gameEvent = TIMEOUT_DISCARD;
	static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
	std::uniform_int_distribution<int> distribution(0, 1);
	// Prepare message
	json message;
	message["MessageType"] = "MERCHANT_DISCARD_CARDS_TIMEOUT";
	message["PlayerName"] = m_playerList[0]->getPlayerName();
	for (auto& card : m_selectedCards) {
		message["Card"] = Card::m_cardNameToString.at(card->getCardType());
		int randomNum = distribution(generator);
		if (randomNum == 0) {
			message["Pile"] = "LEFT_DISCARD_PILE";
		}
		else if (randomNum == 1) {
			message["Pile"] = "RIGHT_DISCARD_PILE";
		}
		std::string messageString = message.dump();
		Network::getInstance().sendMessage(messageString);
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

	std::lock_guard<std::mutex> lock(m_userHandMutex);
	m_userHand.push_back(new Card(card));

	return true;
}

bool Game::removeFromUserHand(uint8_t index)
{
	std::lock_guard<std::mutex> lock(m_userHandMutex);
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
			std::lock_guard<std::mutex> lock(m_userHandMutex);
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
		if (m_ButtonLeft.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && !m_discardDone) {
			// Clear all leftover selected cards
			for (auto& card : m_selectedCards) {
				delete card;
			}
			m_selectedCardsMutex.lock();
			m_selectedCards.clear();
			m_selectedCardsMutex.unlock();
			// Move presented cards from user hand to discard area
			for (int i = 0; i < m_userHand.size(); i++) {
				if (m_userHand[i]->isSelected()) {
					m_selectedCardsMutex.lock();
					m_selectedCards.push_back(m_userHand[i]);
					m_selectedCardsMutex.unlock();

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

			// Position guide text
			m_textMutex.lock();
			m_guideText.setString("Draw new cards from any deck");
			m_guideText.setPosition(668.f, 180.f);
			m_textMutex.unlock();

			// Change state machine
			m_gameEvent = WITHDRAW;
		}

		// Onclick present event
		if (m_ButtonRight.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			m_tablet->showTablet(Tablet::Type::GIVE_BAG, m_playerList[0]->getPlayerMoney());
			m_gameEvent = Game::PRESENT;
		}
	}

	// If it is user's sheriff turn, the Inspect and Pass button are interactable
	if (m_playerList[0]->isSheriff() && m_playerList[0]->isInTurn() && m_gameEvent == SHERIFF_TURN) {
		// If player press "Inspect" button
		if (m_ButtonLeft.getGlobalBounds().contains(mousePosXY)) {
			// Send message to server
			json message;
			message["MessageType"] = "SHERIFF_CHECK";
			message["PlayerName"] = m_playerList[0]->getPlayerName();
			std::string messageString = message.dump();
			Network::getInstance().sendMessage(messageString);
			// Wait for server confirmation
			m_gameEvent = IDLE;
		}

		// If player press "Pass" button
		else if (m_ButtonRight.getGlobalBounds().contains(mousePosXY)) {
			// Send message to server
			json message;
			message["MessageType"] = "SHERIFF_PASS";
			message["PlayerName"] = m_playerList[0]->getPlayerName();
			std::string messageString = message.dump();
			Network::getInstance().sendMessage(messageString);
			// Wait for server confirmation
			m_gameEvent = IDLE;
		}
	}

	// If a player's catalog is clicked, show that player's info
	for (auto& player : m_playerList) {
		if (player->getInfoTabIcon().getGlobalBounds().contains(mousePosXY) && m_gameEvent != DISCONNECTED) {
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
				std::lock_guard<std::mutex> lock(m_selectedCardsMutex);
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
			std::lock_guard<std::mutex> lock(m_selectedCardsMutex);
			card->getCard().setPosition(mousePosXY + m_dragOffset);
		}
	}

	return true;
}

bool Game::handleMouseHover(sf::Vector2f mousePosXY)
{
	if (m_ButtonLeft.getGlobalBounds().contains(mousePosXY) && m_playerList[0]->isInTurn() && m_gameEvent == DEFAULT && !m_discardDone) {
		m_glowShader.setUniform("isHovered", true);
	}
	else {
		m_glowShader.setUniform("isHovered", false);
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
				std::lock_guard<std::mutex> lock(m_selectedCardsMutex);
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
			if (m_ev.mouseButton.button == sf::Mouse::Left && m_gameEvent != IDLE) {
				handleMouseClick(mousePosXY);
			}
			break;

		case sf::Event::MouseMoved:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_gameEvent != IDLE) {
				handleMouseDrag(mousePosXY);
			}
			else {
				handleMouseHover(mousePosXY);
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
	m_userHandMutex.lock();
	m_selectedCardsMutex.lock();
	m_dummyCardsMutex.lock();

	m_animationPlayer.update(deltaTime);
	updateUserHandAnimation(deltaTime);

	m_userHandMutex.unlock();
	m_selectedCardsMutex.unlock();
	m_dummyCardsMutex.unlock();

	// Update the timer if needed
	m_timer->update();

	switch (m_gameEvent)
	{
	case Game::DISCONNECTED:
		m_disconnectPopup->show();
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		// If tablet is shown, it is interactable
		m_tablet->update();

		// If connection is re-established, restore game state
		if (attemptReconnect(deltaTime)) {
			m_disconnectPopup->hide();
			m_gameEvent = m_lastUpdatedGameEvent;
		}

		break;

	case Game::IDLE:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
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
		// If tablet is shown, it is interactable
		m_tablet->update();

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
		// If tablet is shown, it is interactable
		m_tablet->update();

		setupPlayerUI();

		// If done withdrawing, change game state
		if (m_userHand.size() == 6U) {
			// Position guide text
			m_textMutex.lock();
			m_guideText.setString("Drag discarded cards to left or right pile");
			m_guideText.setPosition(586.f, 180.f);
			m_textMutex.unlock();

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

	case Game::SHERIFF_TURN:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		// If tablet is shown, it is interactable
		m_tablet->update();

		setupPlayerUI();

		break;

	case Game::REVEAL:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		// If tablet is shown, it is interactable
		m_tablet->update();

		setupPlayerUI();

		// If done revealing, update game state
		if (m_revealingDone) {
			m_revealingDone = false;
			// Wait for next turn nofification from server
			m_gameEvent = IDLE;
		}

	case Game::TIMEOUT_WITHDRAW:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}

		if (m_userHand.size() == 6U) {
			handleTimeoutDiscard();
			m_gameEvent = TIMEOUT_DISCARD;
		}

		break;

	case Game::TIMEOUT_DISCARD:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}

		if (m_selectedCards.empty()) {
			m_gameEvent = IDLE;
			// Restructure server's timeout message type
			json serverMessage;
			serverMessage["MessageType"] = "PLAYER_TIMEOUT";
			// Respond message
			Network::getInstance().respondMessage(serverMessage);
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
	Player* sheriffPlayer = nullptr;
	for (const auto& player : m_playerList) {
		m_window->draw(player->getAvatar());
		m_window->draw(player->getNameText());
		m_window->draw(player->getInfoTabIcon());
		if (player->isSheriff()) {
			sheriffPlayer = player;
			m_window->draw(player->getSheriffBadge());
		}
		else {
			m_window->draw(player->getBagIcon());
		}
		if (player->isInTurn()) {
			m_window->draw(player->getTurnIndicator());
		}
	}

	// Buttons
	m_window->draw(m_ButtonLeft, &m_glowShader);
	m_window->draw(m_ButtonLeftText);
	m_window->draw(m_ButtonRight);
	m_window->draw(m_ButtonRightText);

	// A mask to focus on withdraw event, filter out avatars, buttons,...
	std::lock_guard<std::mutex> lockText(m_textMutex);
	if (m_gameEvent == WITHDRAW) {
		m_window->draw(m_withdrawEventMask);
		m_window->draw(m_guideText);
	}

	// User hand
	std::lock_guard<std::mutex> lockUserHand(m_userHandMutex);
	for (const auto& card : m_userHand) {
		m_window->draw(card->getCard());
	}

	// Draw decks if it is not sheriff turn
	if (m_gameEvent != SHERIFF_TURN) {
		// Decks
		m_window->draw(m_deck->getMainDeck());
		// A mask to focus on discard event, filter out the main deck
		if (m_gameEvent == DISCARD || (m_gameEvent == IDLE && !m_tablet->isTabletVisible())) {
			m_window->draw(m_discardEventMask);
			m_window->draw(m_guideText);
		}
		m_window->draw(m_deck->getDiscardDeckLeft());
		m_window->draw(m_deck->getDiscardDeckRight());
	}

	// Draw Sheriff event objects
	if (m_gameEvent == SHERIFF_TURN || m_gameEvent == REVEAL) {
		// Draw objects
		m_window->draw(m_SheriffEventMask);
		m_window->draw(m_goodsReportText);

		// Draw highlighted sheriff and merchant
		m_window->draw(sheriffPlayer->getAvatar());
		m_window->draw(sheriffPlayer->getNameText());
		m_window->draw(sheriffPlayer->getInfoTabIcon());
		m_window->draw(sheriffPlayer->getSheriffBadge());
		m_window->draw(sheriffPlayer->getTurnIndicator());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getAvatar());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getNameText());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getInfoTabIcon());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getBagIcon());

		// If it is user's sheriff turn, highlight buttons
		if (m_playerList[0]->isInTurn() && m_gameEvent == SHERIFF_TURN) {
			m_window->draw(m_ButtonLeft);
			m_window->draw(m_ButtonLeftText);
			m_window->draw(m_ButtonRight);
			m_window->draw(m_ButtonRightText);
		}

		// If reveal, show info text
		if (m_gameEvent == REVEAL) {
			m_window->draw(m_infoText);
		}

		// Draw money
		m_window->draw(m_moneyIcon);
		m_window->draw(m_moneyText);
	}

	// Draw dummy card animation if needed
	for (auto& card : m_dummyCards) {
		m_dummyCardsMutex.lock();
		m_window->draw(card->getCard());	
		m_dummyCardsMutex.unlock();
	}

	// Selected cards, for discard,present...
	Card* draggedCard = nullptr;
	std::lock_guard<std::mutex> lockSelectedCards(m_selectedCardsMutex);
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

	// Render popup if experience disconection
	if (m_gameEvent == DISCONNECTED) {
		m_disconnectPopup->render();
	}

	m_window->display();

	return true;
}

bool Game::setupPlayerUI()
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->isUserPlayer()) {
			// The buttons are colored only when it is user's turn
			m_ButtonLeft.setFillColor(m_playerList[i]->isInTurn() && (!m_discardDone) ? sf::Color::Red : sf::Color(128, 128, 128));
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

	// Disconnected from server
	if (jsonMessage["MessageType"] == "SERVER_DISCONNECTED") {
		m_disconnectPopup->setMessage("Server disconnected.\nAttempting reconnection...");
		m_lastUpdatedGameEvent = m_gameEvent;
		m_elapsedTime = 0.f;
		m_gameEvent = DISCONNECTED;
	}

	// Game start round
	if (jsonMessage["MessageType"] == "GAME_START_ROUND") {
		// T.B.D
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Game deals role, only care to switch status for sheriff player, the rest are merchants
	if (jsonMessage["MessageType"] == "GAME_DEALS_ROLE") {
		if (jsonMessage["Role"] == "SHERIFF") {
			for (auto& player : m_playerList) {
				if (player->getPlayerName() == jsonMessage["PlayerName"]) {
					player->setSheriffStatus(true);
				}
				else {
					player->setSheriffStatus(false);
				}
			}
		}
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Infomation of discard piles
	if (jsonMessage["MessageType"] == "DISCARD_PILES") {
		// Push left pile to stack
		for (auto it = jsonMessage["LeftPile"].begin(); it != jsonMessage["LeftPile"].end(); it++) {
			std::string cardName = *it;
			m_deck->getStackLeft().push(new Card(Card::m_stringToCardName.at(cardName)));
			m_deck->setDiscardDeckLeftTexture(m_deck->getStackLeft().top()->getCardType());
		}
		// Push right pile to stack
		for (auto it = jsonMessage["RightPile"].begin(); it != jsonMessage["RightPile"].end(); it++) {
			std::string cardName = *it;
			m_deck->getStackRight().push(new Card(Card::m_stringToCardName.at(cardName)));
			m_deck->setDiscardDeckRightTexture(m_deck->getStackRight().top()->getCardType());
		}
		// Send response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Game Deals Cards
	if (jsonMessage["MessageType"] == "GAME_DEALS_CARDS" && jsonMessage.contains("Cards")) {
		int index = 0;
		// Get all Card types
		for (auto it = jsonMessage["Cards"].begin(); it != jsonMessage["Cards"].end(); it++) {
			if (m_userHand.size() >= 6U) break;	// Limit guard
			// Get card type as string
			std::string cardName = *it;

			// Add to user hand
			addToUserHand(Card::m_stringToCardName.at(cardName));

			// Add drawing animation of that card from the main deck
			std::lock_guard<std::mutex> lock(m_userHandMutex);
			m_userHand[m_userHand.size() - 1]->getCard().setPosition(m_deck->getMainDeck().getPosition() + sf::Vector2f(75, 100));
			m_userHand[m_userHand.size() - 1]->getCard().setScale(sf::Vector2f(0.f, 0.f));
			float posX = 520.f + ((m_userHand.size()-1) % 6) * 150.f;
			float delay = index * 0.3;
			// Add animation, after animation done, send response message
			if (m_userHand.size() == 6U) {
				m_animationPlayer.addAnimation(new Animation(m_userHand[m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
					0.1, sf::Vector2f(posX, 635.f), 1.f, delay, [this, jsonMessage] 
				{ Network::getInstance().respondMessage(jsonMessage); }));
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
		for (int i = 0; i < m_playerList.size(); i++) {
			// Reset turn UI
			m_playerList[i]->setTurn(false);
			// Handle logic if find player's turn
			if (m_playerList[i]->getPlayerName() == playerName) {
				// Hide tablet if needed
				m_tablet->hideTablet();

				m_playerList[i]->setTurn(true);

				// Start the timer if user player
				if (m_playerList[i]->isUserPlayer()) {
					m_timer->reset();
					m_timer->start();
				}

				// If it is sheriff's turn, setup animations and UI of giving bag
				if (m_playerList[i]->isSheriff() && !m_dummyCards.empty()) {
					// Change game state
					m_gameEvent = SHERIFF_TURN;
					// Setup initial position to make sure the cards are centered
					float totalWidth = m_dummyCards.size() * 135.f + (m_dummyCards.size() - 1) * 15.f;
					float posXOffset = ((1920 - totalWidth) / 2.f) + 67.5;
					// Animation
					for (int i = 0; i < m_dummyCards.size(); i++) {
						float posX = posXOffset + i * (135.f + 15.f);
						std::lock_guard<std::mutex> lock(m_dummyCardsMutex);
						m_dummyCards[i]->getCard().setOrigin(67.5, 97.5);
						m_animationPlayer.addAnimation(new Animation(m_dummyCards[i]->getCard(), Animation::Type::MOVE_AND_SCALE, 0.3,
							sf::Vector2f(posX, 395.f), 1.f));
					}

					// Set infomation
					m_textMutex.lock();
					m_goodsReportText.setString("Report: " + Card::m_cardNameToString.at(m_goodsReport));
					m_goodsReportText.setPosition((1920.f - m_goodsReportText.getGlobalBounds().width) / 2, 215.f);
					m_moneyIcon.setPosition(910.f, 515.f);
					m_moneyIcon.setScale(1.f, 1.f);
					m_moneyText.setString(std::to_string(m_bribeAmount));
					m_moneyText.setFillColor(sf::Color::White);
					m_moneyText.setCharacterSize(48);
					m_moneyText.setScale(1.f, 1.f);
					m_moneyText.setPosition(1030.f, 534.f);
					m_textMutex.unlock();

					// Temporary hide all decks and user cards
					m_deck->getDiscardDeckLeft().setScale(0.f, 0.f);
					m_deck->getDiscardDeckRight().setScale(0.f, 0.f);
					m_deck->getMainDeck().setScale(0.f, 0.f);
					for (auto& card : m_userHand) {
						std::lock_guard<std::mutex> lock(m_userHandMutex);
						card->getCard().setScale(0.f, 0.f);
					}
				}
				// If it is a merchant's turn, reset all previous stored goods infomation, for safety measure
				else {
					m_tablet->resetOptions();
					m_dummyCards.clear();
					m_selectedCards.clear();

					m_discardDone = false;
					m_gameEvent = DEFAULT;

					// Show decks and user cards again
					m_deck->getDiscardDeckLeft().setScale(1.f, 1.f);
					m_deck->getDiscardDeckRight().setScale(1.f, 1.f);
					m_deck->getMainDeck().setScale(1.f, 1.f);
					for (auto& card : m_userHand) {
						std::lock_guard<std::mutex> lock(m_userHandMutex);
						card->getCard().setScale(1.f, 1.f);
					}
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
					m_selectedCardsMutex.lock();
					m_selectedCards.push_back(m_userHand[i]);
					m_selectedCardsMutex.unlock();
					removeFromUserHand(i);
					i--;
					// Push a dummy card to render next Sheriff turn
					m_dummyCards.push_back(new Card(Card::UNKNOWN));
					m_dummyCards[m_dummyCards.size() - 1]->getCard().setScale(0.f, 0.f);
					m_dummyCards[m_dummyCards.size() - 1]->getCard().setPosition(m_playerList[0]->getBagIcon().getPosition() + sf::Vector2f(40.f, 40.f));
				}
			}
			// Animation highlighting bag
			m_animationPlayer.addAnimation(new Animation(m_playerList[0]->getBagIcon(), Animation::Type::SCALE, 1.2, 0.25, 0.2, 
				[this, jsonMessage]
			{
				m_animationPlayer.addAnimation(new Animation(m_playerList[0]->getBagIcon(), Animation::Type::SCALE, 1.f, 0.25, 0.f, 
					[this, jsonMessage]
				{
					// Send a response message
					Network::getInstance().respondMessage(jsonMessage);
				}));
			}));
			for (auto& card : m_selectedCards) {
				// Animation put cards into bag
				m_animationPlayer.addAnimation(new Animation(card->getCard(), Animation::Type::MOVE_AND_SCALE, 0.3f,
					m_playerList[0]->getBagIcon().getPosition() + sf::Vector2f(40.f, 40.f), 0.f));
			}
			// Store information of merchant giving bag for next Sheriff turn
			m_MerchantShowingBagIndex = 0;
			// Hide tablet
			m_tablet->hideTablet();
			// Re-arrange userhand
			userHandUI();
			m_gameEvent = DEFAULT;
		}
	}

	// Server confirm give bag event of opponents
	if (jsonMessage["MessageType"] == "MERCHANT_GIVE_BAG") {
		for (int i = 0; i < m_playerList.size(); i++) {
			if (m_playerList[i]->getPlayerName() == jsonMessage["PlayerName"]) {
				// Clear dummy cards to store new ones. Just a safety measure
				for (auto& card : m_dummyCards) {
					delete card;
				}
				m_dummyCards.clear();

				// Store data to render the next Sheriff turn
				m_goodsReport = Card::m_stringToCardName.at(jsonMessage["Report"]);
				m_bribeAmount = jsonMessage["Fee"];
				std::string amount = jsonMessage["Amount"];
				int goodsAmount = std::stoi(amount);
				m_MerchantShowingBagIndex = i;

				// Add animation put cards to bag
				for (int j = 0; j < goodsAmount; j++) {
					m_dummyCards.push_back(new Card(Card::UNKNOWN));
				}
				for (int j = 0; j < m_dummyCards.size(); j++) {
					m_dummyCards[j]->getCard().setScale(0.4, 0.4);
					m_dummyCards[j]->getCard().setPosition(m_playerList[i]->getAvatar().getPosition() + sf::Vector2f(30.f, 30.f));
					m_animationPlayer.addAnimation(new Animation(m_dummyCards[j]->getCard(), Animation::Type::MOVE_AND_SCALE, 0.5,
						m_playerList[i]->getBagIcon().getPosition() + sf::Vector2f(30.f, 30.f), 0.f));
				}
				// Animation highlighting bag
				m_animationPlayer.addAnimation(new Animation(m_playerList[i]->getBagIcon(), Animation::Type::SCALE, 1.2, 0.2, 0.3,
					[this, jsonMessage, i]
				{
					m_animationPlayer.addAnimation(new Animation(m_playerList[i]->getBagIcon(), Animation::Type::SCALE, 1.f, 0.2, 0.f,
						[this, jsonMessage]
					{
						// Send a response message
						Network::getInstance().respondMessage(jsonMessage);
					}));
				}));
				break;
			}
		}
	}

	// Server confirm player withdrawing
	if (jsonMessage["MessageType"] == "MERCHANT_WITHDRAW_CARDS_RESPONSE") {
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

	// Server notify Sheriff Check
	if (jsonMessage["MessageType"] == "SHERIFF_CHECK_RESPONSE") {
		// Set game state
		m_gameEvent = REVEAL;
		// Set info text
		m_textMutex.lock();
		m_infoText.setString("Sheriff chose to inspect the goods!");
		m_infoText.setPosition((1920.f - m_infoText.getGlobalBounds().width) / 2, 680.f);
		m_textMutex.unlock();
		// Handle animations
		handleSheriffInspectEvent(jsonMessage);
	}

	// Server notify Sheriff Check
	if (jsonMessage["MessageType"] == "SHERIFF_PASS_RESPONSE") {
		// Set game state
		m_gameEvent = REVEAL;
		// If the timer is running, turn it off
		m_timer->stop();
		// Set info text
		m_textMutex.lock();
		m_infoText.setString("Sheriff chose to let the goods pass!");
		m_infoText.setPosition((1920.f - m_infoText.getGlobalBounds().width) / 2, 680.f);
		m_textMutex.unlock();
		// Handle animations
		handleSheriffPassEvent(jsonMessage);
	}

	// Server notify timeout
	if (jsonMessage["MessageType"] == "PLAYER_TIMEOUT" && jsonMessage["PlayerName"] == m_playerList[0]->getPlayerName()) {
		// Stop the timer
		m_timer->stop();

		// If player is in the middle of withdraw/discard event, randomize what's left
		if (m_gameEvent == WITHDRAW) {
			handleTimeoutWithdraw();
		}
		else if (m_gameEvent == DISCARD) {
			handleTimeoutDiscard();
		}
		// If not, send response message and wait for new turn
		else {
			m_gameEvent = TIMEOUT_DISCARD;
			Network::getInstance().respondMessage(jsonMessage);
		}
	}
}




