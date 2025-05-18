#include "../include/Core/Game.h"
#include "../include/Core/GameLogic.h"
#include "../include/Core/GameState.h"
//#include <Windows.h>
#include <string.h>

#define USER_PLAYER_INDEX	0U

void Game::initVariables(std::vector<Player*> playerList)
{
	if (!m_backgroundTexture.loadFromFile("assets/Images/Background.png") ||
		!m_cardsHolderTexture.loadFromFile("assets/Images/CardsHolder.png") ||
		!m_backgroundClothTexture.loadFromFile("assets/Images/Background_cloth.png") ) {
		std::cerr << "Error loading background texture!\n";
	}
	m_backgroundTexture.setSmooth(true);
	m_cardsHolderTexture.setSmooth(true);

	if (!m_backgroundMusic.openFromFile("assets/Sound/IngameBackgroundMusic.wav")) {
		std::cerr << "Error loading background music!\n";
	}
	m_backgroundMusic.setVolume(70);
	m_backgroundMusic.setLoop(true);

	if (!m_moneyIconTexture.loadFromFile("assets/Images/Tokens/MoneyIcon.png") ||
		!m_soundIconTexture.loadFromFile("assets/Images/SoundIndicator.png") ||
		!m_noSoundIconTexture.loadFromFile("assets/Images/NoSoundIndicator.png") ||
		!m_discardButtonGrayTexture.loadFromFile("assets/Images/Buttons/ButtonDiscardGray.png") ||
		!m_discardButtonHighlightTexture.loadFromFile("assets/Images/Buttons/ButtonDiscardHighLight.png") ||
		!m_discardButtonTexture.loadFromFile("assets/Images/Buttons/ButtonDiscard.png") || 
		!m_presentButtonGrayTexture.loadFromFile("assets/Images/Buttons/ButtonPresentGray.png") || 
		!m_presentButtonHighLightTexture.loadFromFile("assets/Images/Buttons/ButtonPresentHighLight.png") || 
		!m_presentButtonTexture.loadFromFile("assets/Images/Buttons/ButtonPresent.png") ||
		!m_InspectButtonGrayTexture.loadFromFile("assets/Images/Buttons/ButtonInspectGray.png") ||
		!m_InspectButtonHighlightTexture.loadFromFile("assets/Images/Buttons/ButtonInspectHighLight.png") ||
		!m_InspectButtonTexture.loadFromFile("assets/Images/Buttons/ButtonInspect.png") || 
		!m_PassButtonGrayTexture.loadFromFile("assets/Images/Buttons/ButtonPassGray.png") ||
		!m_PassButtonHighlightTexture.loadFromFile("assets/Images/Buttons/ButtonPassHighLight.png") ||
		!m_PassButtonTexture.loadFromFile("assets/Images/Buttons/ButtonPass.png") )
	{	
		std::cerr << "Error loading game texture!\n";
	}

	if (!m_font.loadFromFile("assets/arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	//if (!m_glowShader.loadFromFile("assets/Shaders/glow.frag", sf::Shader::Fragment)) {
	//	std::cerr << "Error loading shader!\n";
	//}

	m_gameEvent = Game::DEFAULT;
	m_lastUpdatedGameEvent = DEFAULT;
	m_elapsedTime = 0.f;
	m_anyCardSelected = false;
	m_anyCardDragged = false;
	m_discardDone = false;
	m_revealingDone = false;
	m_window = nullptr;
	m_dragOffset = sf::Vector2f(0.f, 0.f);
	m_playerList = playerList;
	m_deck = nullptr;
	m_tablet = nullptr;
	m_timer = nullptr;
	m_disconnectPopup = nullptr;
	m_MerchantShowingBagIndex = 0;

	m_gameLogic = std::make_unique<GameLogic>(this);
}

void Game::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	m_backgroundMusic.play();

	for (int i = 1; i < m_playerList.size(); i++) {
		// Positioning players
		float xOffset = -210.f + (i % 6) * 380.f; // Distribute evenly horizontally
		m_playerList[i]->initPlayer(xOffset, 70.f);
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
	
	m_backgroundCloth.setPosition(sf::Vector2f(0.f, 0.f));
	m_backgroundCloth.setFillColor(sf::Color::White);
	m_backgroundCloth.setSize(sf::Vector2f(1920.f, 1080.f));
	m_backgroundCloth.setTexture(&m_backgroundClothTexture);

	m_cardsHolder.setPosition(sf::Vector2f(0.f, 0.f));
	m_cardsHolder.setFillColor(sf::Color::White);
	m_cardsHolder.setSize(sf::Vector2f(1920.f, 1080.f));
	m_cardsHolder.setTexture(&m_cardsHolderTexture);

	m_withdrawEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_withdrawEventMask.setFillColor(sf::Color(0, 0, 0, 180));
	m_discardEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_discardEventMask.setFillColor(sf::Color(0, 0, 0, 180));
	m_SheriffEventMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_SheriffEventMask.setFillColor(sf::Color(0, 0, 0, 180));

	// Init shaders
	//m_glowShader.setUniform("isHovered", false);
	//m_glowShader.setUniform("outlineColor", sf::Glsl::Vec4(1.0, 1.0, 1.0, 1.0));
	//m_glowShader.setUniform("glowStrength", 0.35f); // Adjust glow thickness

	// Configure the button left
	m_ButtonLeft.setSize(sf::Vector2f(170.f, 70.f));
	m_ButtonLeft.setFillColor(sf::Color::White);
	//// Configure the text
	//m_ButtonLeftText.setFont(m_font);
	//m_ButtonLeftText.setString("Discard");
	//m_ButtonLeftText.setCharacterSize(28); 
	//m_ButtonLeftText.setFillColor(sf::Color::White);
	//// Center the text within the button
	//sf::FloatRect textBoundsLeft = m_ButtonLeftText.getLocalBounds();
	//m_ButtonLeftText.setOrigin(textBoundsLeft.left + textBoundsLeft.width / 2, textBoundsLeft.top + textBoundsLeft.height / 2);

	// Configure the button right
	m_ButtonRight.setSize(sf::Vector2f(170.f, 70.f));
	m_ButtonRight.setFillColor(sf::Color::White);
	//// Configure the text
	//m_ButtonRightText.setFont(m_font);
	//m_ButtonRightText.setString("Present");
	//m_ButtonRightText.setCharacterSize(28);
	//m_ButtonRightText.setFillColor(sf::Color::White);
	//// Center the text within the button
	//sf::FloatRect textBoundsRight = m_ButtonRightText.getLocalBounds();
	//m_ButtonRightText.setOrigin(textBoundsRight.left + textBoundsRight.width / 2, textBoundsRight.top + textBoundsRight.height / 2);

	// Configure Sound indicator
	m_soundIcon.setSize(sf::Vector2f(80.f, 80.f));
	m_soundIcon.setFillColor(sf::Color::White);
	m_soundIcon.setTexture(&m_soundIconTexture);
	m_soundIcon.setPosition(sf::Vector2f(70.f, 940.f));

	// Setup bribe amount, goods report
	m_goodsReportText.setFont(m_font);
	m_goodsReportText.setCharacterSize(48);
	m_goodsReportText.setFillColor(sf::Color::White);
	m_goodsReportIcon.setSize(sf::Vector2f(80.f, 80.f));
	m_goodsReportIcon.setFillColor(sf::Color::White);
	m_moneyText.setFont(m_font);
	m_moneyText.setCharacterSize(48);
	m_moneyText.setFillColor(sf::Color::White);
	m_moneyIcon.setSize(sf::Vector2f(100.f, 100.f));
	m_moneyIcon.setFillColor(sf::Color::White);
	m_moneyIcon.setTexture(&m_moneyIconTexture);
	m_bribedGoodsText.setFont(m_font);
	m_bribedGoodsText.setCharacterSize(48);
	m_bribedGoodsText.setFillColor(sf::Color::White);

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
	//m_ButtonLeftText.setPosition(m_ButtonLeft.getPosition().x + m_ButtonLeft.getSize().x / 2,
	//	m_ButtonLeft.getPosition().y + m_ButtonLeft.getSize().y / 2);
	//m_ButtonRightText.setPosition(m_ButtonRight.getPosition().x + m_ButtonRight.getSize().x / 2,
	//	m_ButtonRight.getPosition().y + m_ButtonRight.getSize().y / 2);

	// Positioning bribe amount, goods report
	//m_goodsReportText.setPosition(755.f, 200.f);
	//m_moneyIcon.setPosition(910.f, 515.f);
	//m_moneyText.setPosition(1030.f, 534.f);

	// Initial draw user hand
	userHandUI();
}


Game::Game(std::vector<Player*> playerList)
{
	initVariables(playerList);
	initWindow();
}

Game::~Game()
{
	for (int i = 0; i < m_userHand.size(); i++) {
		delete m_userHand[i];
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

std::vector<Player*> Game::getPlayerList() const
{
	return m_playerList;
}

bool Game::handleMouseClick(sf::Vector2f mousePosXY)
{
	// Select cards to discard or present
	m_anyCardSelected = false;
	for (int i = 0; i < m_userHand.size(); i++) {
		if (m_userHand[i]->getCard().getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_gameEvent == DEFAULT) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			m_userHand[i]->getAnimationPlayer().cancel();
			std::lock_guard<std::mutex> lock(m_userHandMutex);
			m_userHand[i]->setSelected(!m_userHand[i]->isSelected());
			std::cout << "Card " + Card::m_cardNameToString.at(m_userHand[i]->getCardType()) + ": " + (m_userHand[i]->isSelected() ? "Selected\n" : "Not selected\n");

			float posX = 520.f + (i % 6) * 150.f;
			float posY = 635.f;
			m_userHand[i]->animationMove(0.1,
				sf::Vector2f(posX, m_userHand[i]->isSelected() ? (posY - 35.f) : posY));
		}
		
		// Check if any card is selected for other interactions
		if (m_userHand[i]->isSelected()) {
			m_anyCardSelected = true;
		}
	}

	// If a player's catalog is clicked, show that player's info
	for (auto& player : m_playerList) {
		if (player->getInfoTabIcon().getGlobalBounds().contains(mousePosXY) && m_gameEvent != DISCONNECTED) {
			if (m_gameEvent != REVEAL) {
				// Play a sound
				m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

				// Show tablet
				m_tablet->showTablet(Tablet::Type::INFO, player->getPlayerMoney(), player);
			}

			else {
				// Play sound indicating that this is not currently available
				m_soundPlayer.play("Locked.wav", Sound::Type::EFFECT);
			}


			return true;
		}
	}

	// If any card is selected and it is user's merchant turn, the Discard and Present buttons are interactable
	if (m_anyCardSelected && m_playerList[USER_PLAYER_INDEX]->isInTurn() && !m_playerList[USER_PLAYER_INDEX]->isSheriff() && m_gameEvent == DEFAULT) {
		// Onclick discard event
		if (m_ButtonLeft.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && !m_discardDone) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			// Clear all leftover selected cards
			for (auto& card : m_selectedCards) {
				delete card;
			}
			m_selectedCardsMutex.lock();
			m_selectedCards.clear();
			m_selectedCardsMutex.unlock();

			json message;

			// Count number of selected cards
			int numberOfSelectedCards = 0;
			for (int i = 0; i < m_userHand.size(); i++) {
				if (m_userHand[i]->isSelected()) {
					numberOfSelectedCards++;
					message["Cards"].push_back(Card::m_cardNameToString.at(m_userHand[i]->getCardType()));
				}
			}

			// Send message to request discard
			message["MessageType"] = "MERCHANT_DISCARD_REQUEST";
			message["NumberOfCards"] = std::to_string(numberOfSelectedCards);
			message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
			std::string messageString = message.dump();
			Network::getInstance().sendMessage(messageString);

			m_gameEvent = IDLE;
			return true;
		}

		// Onclick present event
		else if (m_ButtonRight.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			m_tablet->showTablet(Tablet::Type::GIVE_BAG, m_playerList[USER_PLAYER_INDEX]->getPlayerMoney());
			m_gameEvent = Game::PRESENT;
			return true;
		}
	}

	// If it is user's sheriff turn, the Inspect and Pass button are interactable
	if (m_playerList[USER_PLAYER_INDEX]->isSheriff() && m_playerList[USER_PLAYER_INDEX]->isInTurn() && m_gameEvent == SHERIFF_TURN) {
		// If player press "Inspect" button
		if (m_ButtonLeft.getGlobalBounds().contains(mousePosXY)) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			// Send message to server
			json message;
			message["MessageType"] = "SHERIFF_CHECK";
			message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
			std::string messageString = message.dump();
			Network::getInstance().sendMessage(messageString);
			// Wait for server confirmation
			m_gameEvent = SHERIFF_STANDBY;
			return true;
		}

		// If player press "Pass" button
		else if (m_ButtonRight.getGlobalBounds().contains(mousePosXY)) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			// Send message to server
			json message;
			message["MessageType"] = "SHERIFF_PASS";
			message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
			std::string messageString = message.dump();
			Network::getInstance().sendMessage(messageString);
			// Wait for server confirmation
			m_gameEvent = SHERIFF_STANDBY;
			return true;
		}
	}

	// Left deck is clicked when withdrawing
	if (m_deck->getDiscardDeckLeft().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		// Play a sound
		m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

		// Only interactable if that deck has any card left
		if (m_deck->getStackLeft().empty()) return false;

		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
		message["Pile"] = "LEFT_DISCARD_PILE";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
		return true;
	}

	// Right deck is clicked when withdrawing
	if (m_deck->getDiscardDeckRight().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		// Play a sound
		m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

		// Only interactable if that deck has any card left
		if (m_deck->getStackRight().empty()) return false;

		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
		message["Pile"] = "RIGHT_DISCARD_PILE";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
		return true;
	}

	// Main deck is clicked when withdrawing
	if (m_deck->getMainDeck().getGlobalBounds().contains(mousePosXY) && m_gameEvent == WITHDRAW) {
		// Play a sound
		m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS";
		message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
		message["Pile"] = "MAIN_DECK";

		std::string messageString = message.dump();
		Network::sendMessage(messageString);
		return true;
	}

	// A selected card is clicked when discarding
	for (auto& card : m_selectedCards) {
		if (card->getCard().getGlobalBounds().contains(mousePosXY) && m_gameEvent == DISCARD) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			// Start the drag and save static position
			if (!card->isDragging() && !m_anyCardDragged) {
				std::lock_guard<std::mutex> lock(m_selectedCardsMutex);
				card->setupCardUI(card->getCard().getPosition().x, card->getCard().getPosition().y, sf::Vector2f(0.72, 0.72));
				card->getCard().setScale(1, 1);
				m_dragOffset = card->getCard().getPosition() - mousePosXY;
				m_anyCardDragged = true;
				card->setDragging(true);
				return true;
			}
		}
	}

	// If clicked on sound icon, toggle play/stop background music
	if (m_soundIcon.getGlobalBounds().contains(mousePosXY)) {
		// Play a sound
		m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

		if (m_backgroundMusic.getStatus() == sf::SoundSource::Status::Playing) {
			m_backgroundMusic.stop();
			m_soundIcon.setTexture(&m_noSoundIconTexture);
		}
		else {
			m_backgroundMusic.play();
			m_soundIcon.setTexture(&m_soundIconTexture);
		}
		return true;
	}

	return false;
}

bool Game::handleMouseDrag(sf::Vector2f mousePosXY)
{
	for (auto& card : m_selectedCards) {
		if (card->isDragging() && m_gameEvent == DISCARD) {
			std::lock_guard<std::mutex> lock(m_selectedCardsMutex);
			card->getCard().setPosition(mousePosXY + m_dragOffset);
		}
	}

	return false;
}

bool Game::handleMouseHover(sf::Vector2f mousePosXY)
{
	// Highlight hovered card
	for (int i = 0; i < m_userHand.size(); i++) {
		float posX = 520.f + (i % 6) * 150.f;
		if (m_userHand[i]->getCard().getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_gameEvent == DEFAULT
			&& !m_userHand[i]->isSelected()) {
			if (!m_userHand[i]->isHovered()) {
				m_userHand[i]->animationMove(0.1, sf::Vector2f(posX - 3.f, 630.f), 1.05f);
				m_userHand[i]->setHovering(true);
			}

		}
		else {
			if (m_userHand[i]->isHovered() && !m_userHand[i]->isSelected()) {
				m_userHand[i]->animationMove(0.1, sf::Vector2f(posX, 635.f), 1.f, 0.1);
				m_userHand[i]->setHovering(false);
			}
		}
	}

	// Highlight hovered button
	if (m_ButtonLeft.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_playerList[USER_PLAYER_INDEX]->isInTurn()) {
		if (m_playerList[USER_PLAYER_INDEX]->isSheriff() && m_gameEvent == SHERIFF_TURN) {
			m_ButtonLeft.setTexture(&m_InspectButtonHighlightTexture, true);
		}
		else if ((!m_discardDone) && m_gameEvent == DEFAULT) {
			m_ButtonLeft.setTexture(&m_discardButtonHighlightTexture, true);
		}
	}

	else if (m_ButtonRight.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_playerList[USER_PLAYER_INDEX]->isInTurn()) {
		if (m_playerList[USER_PLAYER_INDEX]->isSheriff() && m_gameEvent == SHERIFF_TURN) {
			m_ButtonRight.setTexture(&m_PassButtonHighlightTexture, true);
		}
		else if (m_gameEvent == DEFAULT){
			m_ButtonRight.setTexture(&m_presentButtonHighLightTexture, true);
		}
	}

	else {
		setupPlayerUI();
	}

	return true;
}

bool Game::handleMouseRelease()
{
	for (int i = 0; i < m_selectedCards.size(); i++) {
		if (m_selectedCards[i]->isDragging() && m_gameEvent != IDLE) {
			// If the dragged card is placed on top of left discard pile
			if (m_selectedCards[i]->getCard().getGlobalBounds().intersects(m_deck->getDiscardDeckLeft().getGlobalBounds())) {
				// Notify server
				json message;
				message["MessageType"] = "MERCHANT_DISCARD_CARDS";
				message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
				message["Pile"] = "LEFT_DISCARD_PILE";
				message["Card"] = Card::m_cardNameToString.at(m_selectedCards[i]->getCardType());
				std::string messageString = message.dump();
				Network::getInstance().sendMessage(messageString);

				// Wait for server confirmation
				m_gameEvent = DISCARD_STANDBY;
			}

			// If the dragged card is placed on top of right discard pile
			else if (m_selectedCards[i]->getCard().getGlobalBounds().intersects(m_deck->getDiscardDeckRight().getGlobalBounds())) {
				// Notify server
				json message;
				message["MessageType"] = "MERCHANT_DISCARD_CARDS";
				message["PlayerName"] = m_playerList[USER_PLAYER_INDEX]->getPlayerName();
				message["Pile"] = "RIGHT_DISCARD_PILE";
				message["Card"] = Card::m_cardNameToString.at(m_selectedCards[i]->getCardType());
				std::string messageString = message.dump();
				Network::getInstance().sendMessage(messageString);

				// Wait for server confirmation
				m_gameEvent = DISCARD_STANDBY;
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
			g_gameState = gameState::INVALID;

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
		if (m_gameLogic->attemptReconnect(deltaTime)) {
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
		//setupPlayerUI();

		// If tablet is shown, it is interactable
		m_tablet->update();

		break;

	case Game::DISCARD:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		// If tablet is shown, it is interactable
		m_tablet->update();

		//setupPlayerUI();

		// If all cards are discarded, change game state
		if (m_selectedCards.empty()) {
			m_gameEvent = DEFAULT;
		}

		break;

	case Game::DISCARD_STANDBY:
		//if (!m_tablet->isTabletVisible()) {
		//	pollEvents();
		//}
		break;

	case Game::WITHDRAW:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		// If tablet is shown, it is interactable
		m_tablet->update();

		//setupPlayerUI();

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
				m_gameLogic->handlePresentEvent();
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

		//setupPlayerUI();

		break;

	case Game::SHERIFF_STANDBY:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}
		break;

	case Game::REVEAL:
		// Opening tablet is unnecessary, so close it
		if (m_tablet->isTabletVisible()) {
			m_tablet->hideTablet();
		}

		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}

		//setupPlayerUI();

		// If done revealing, update game state
		if (m_revealingDone) {
			m_revealingDone = false;
			// Wait for next turn nofification from server
			m_gameEvent = IDLE;
		}

		break;

	case Game::TIMEOUT_WITHDRAW:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}

		if (m_userHand.size() == 6U) {
			m_gameLogic->handleTimeoutDiscard();
			m_gameEvent = TIMEOUT_DISCARD;
		}

		break;

	case Game::TIMEOUT_DISCARD:
		if (!m_tablet->isTabletVisible()) {
			pollEvents();
		}

		if (m_selectedCards.empty()) {
			m_gameEvent = IDLE;
			//// Restructure server's timeout message type
			//json serverMessage;
			//serverMessage["MessageType"] = "PLAYER_TIMEOUT";
			//// Respond message
			//Network::getInstance().respondMessage(serverMessage);
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
	m_window->draw(m_backgroundCloth);
	m_window->draw(m_cardsHolder);

	// Player avatar and stuff
	Player* sheriffPlayer = nullptr;
	for (const auto& player : m_playerList) {
		m_window->draw(player->getAvatar());
		m_window->draw(player->getAvatarFrame());
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
	m_window->draw(m_ButtonLeft);
	//m_window->draw(m_ButtonLeftText);
	m_window->draw(m_ButtonRight);
	//m_window->draw(m_ButtonRightText);

	// A mask to focus on withdraw event, filter out avatars, buttons,...
	std::lock_guard<std::mutex> lockText(m_textMutex);
	if (m_gameEvent == WITHDRAW) {
		m_window->draw(m_withdrawEventMask);
		m_window->draw(m_backgroundCloth);
		m_window->draw(m_guideText);
	}

	// User hand
	std::lock_guard<std::mutex> lockUserHand(m_userHandMutex);
	for (const auto& card : m_userHand) {
		m_window->draw(card->getCard());
	}

	// Decks
	m_window->draw(m_deck->getMainDeck());
	// A mask to focus on discard event, filter out the main deck
	if (m_gameEvent == DISCARD || (m_gameEvent == DISCARD_STANDBY && !m_tablet->isTabletVisible())) {
		m_window->draw(m_discardEventMask);
		m_window->draw(m_backgroundCloth);
		m_window->draw(m_guideText);
	}
	m_window->draw(m_deck->getDiscardDeckLeft());
	m_window->draw(m_deck->getDiscardDeckRight());

	// Draw Sheriff event objects
	if (m_gameEvent == SHERIFF_TURN || m_gameEvent == SHERIFF_STANDBY || m_gameEvent == REVEAL) {
		// Draw objects
		m_window->draw(m_SheriffEventMask);
		m_window->draw(m_goodsReportText);
		m_window->draw(m_goodsReportIcon);
		m_window->draw(m_bribedGoodsText);
		//m_window->draw(m_infoText);

		// Draw highlighted sheriff and merchant
		m_window->draw(sheriffPlayer->getAvatar());
		m_window->draw(sheriffPlayer->getAvatarFrame());
		m_window->draw(sheriffPlayer->getNameText());
		m_window->draw(sheriffPlayer->getInfoTabIcon());
		m_window->draw(sheriffPlayer->getSheriffBadge());
		m_window->draw(sheriffPlayer->getTurnIndicator());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getAvatar());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getAvatarFrame());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getNameText());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getInfoTabIcon());
		m_window->draw(m_playerList[m_MerchantShowingBagIndex]->getBagIcon());

		// If it is user's sheriff turn, highlight buttons
		if (m_playerList[USER_PLAYER_INDEX]->isInTurn() && m_gameEvent == SHERIFF_TURN) {
			m_window->draw(m_ButtonLeft);
			//m_window->draw(m_ButtonLeftText);
			m_window->draw(m_ButtonRight);
			//m_window->draw(m_ButtonRightText);
		}

		// If reveal, show info text
		if (m_gameEvent == REVEAL) {
			m_window->draw(m_infoText);
		}

		// Draw bribed cards in reverse order to help render reveal process
		for (int i = m_bribedCards.size() - 1; i >= 0; i--) {
			m_window->draw(m_bribedCards[i]->getCard());
		}

		// Draw money
		m_window->draw(m_moneyIcon);
		m_window->draw(m_moneyText);
	}

	// Draw dummy card animation if needed
	std::lock_guard<std::mutex> lockDummyCards(m_dummyCardsMutex);
	for (auto& card : m_dummyCards) {
		m_window->draw(card->getCard());	
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

	// Render to sound icon
	m_window->draw(m_soundIcon);

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
	// Render button Inspect and Pass if user role is Sheriff
	if (m_playerList[USER_PLAYER_INDEX]->isSheriff()) {
		// The buttons are colored only when it is user's turn
		m_ButtonLeft.setTexture(m_playerList[USER_PLAYER_INDEX]->isInTurn() ? (&m_InspectButtonTexture) : (&m_InspectButtonGrayTexture), true);
		m_ButtonRight.setTexture(m_playerList[USER_PLAYER_INDEX]->isInTurn() ? (&m_PassButtonTexture) : (&m_PassButtonGrayTexture), true);
	}
	// Render button Discard and Present if user role is Sheriff
	else {
		// The buttons are colored only when it is user's turn
		m_ButtonLeft.setTexture(m_playerList[USER_PLAYER_INDEX]->isInTurn() && (!m_discardDone) ? (&m_discardButtonTexture) : (&m_discardButtonGrayTexture), true);
		m_ButtonRight.setTexture(m_playerList[USER_PLAYER_INDEX]->isInTurn() ? (&m_presentButtonTexture) : (&m_presentButtonGrayTexture), true);
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
			m_animationPlayer.addAnimation(new Animation(m_userHand[i]->getCard(), Animation::Type::MOVE, 0.15, sf::Vector2f(posX, 600.f), 1.f, 0.4));
			m_anyCardSelected = true;
		}
		else {
			//m_userHand[i]->setupCardUI(posX, 635.f);
			m_animationPlayer.addAnimation(new Animation(m_userHand[i]->getCard(), Animation::Type::MOVE, 0.15, sf::Vector2f(posX, 635.f), 1.f, 0.4));
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
		// Store last state to recover
		m_lastUpdatedGameEvent = m_gameEvent;
		m_elapsedTime = 0.f;
		m_gameEvent = DISCONNECTED;
	}

	// Game start round
	else if (jsonMessage["MessageType"] == "GAME_START_ROUND") {
		// T.B.D
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Game deals role, only care to switch status for sheriff player, the rest are merchants
	else if (jsonMessage["MessageType"] == "GAME_DEALS_ROLE") {
		m_gameLogic->handleDealRoleEvent();

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
	else if (jsonMessage["MessageType"] == "DISCARD_PILES") {
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
	else if (jsonMessage["MessageType"] == "GAME_DEALS_CARDS" && jsonMessage.contains("Cards")) {
		// If user hand is already full, respond and return
		if (m_userHand.size() >= 6U) {
			Network::getInstance().respondMessage(jsonMessage);
			return;
		}

		m_gameLogic->handleDealCardsEvent(jsonMessage);
	}

	// Start a new turn
	else if (jsonMessage["MessageType"] == "GAME_START_TURN") {
		std::string playerName = jsonMessage["PlayerName"];

		m_gameLogic->handleStartTurnEvent(playerName);
		setupPlayerUI();

		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server confirm GiveBag event of user
	else if ( jsonMessage["MessageType"] == "MERCHANT_GIVE_BAG_RESPONSE" &&
		jsonMessage["PlayerName"] == m_playerList[USER_PLAYER_INDEX]->getPlayerName() ) 
	{
		if (!m_userHand.empty()) {
			m_gameLogic->handleGiveBagEvent(jsonMessage);
		}
	}

	// Server confirm give bag event of opponents
	else if (jsonMessage["MessageType"] == "MERCHANT_GIVE_BAG") {
		m_gameLogic->handleOpponentGiveBagEvent(jsonMessage);
	}

	// Server accept discard request
	else if (jsonMessage["MessageType"] == "MERCHANT_DISCARD_REQUEST_RESPONSE") {
		// If it is user exchange
		if (jsonMessage["PlayerName"] == m_playerList[USER_PLAYER_INDEX]->getPlayerName()) {
			m_gameLogic->setupExchangeEvent();
			Network::getInstance().respondMessage(jsonMessage);
		}
		// If it is another merchant exchange
		else {
			std::vector<std::string> cardNames;
			for (auto it = jsonMessage["Cards"].begin(); it != jsonMessage["Cards"].end(); it++) {
				std::string cardName = *it;
				cardNames.push_back(cardName);
			}
			for (int i = 1; i < m_playerList.size(); i++) {
				if (jsonMessage["PlayerName"] == m_playerList[i]->getPlayerName()) {
					m_gameLogic->setupOpponentExchangeEvent(i, cardNames);
					Network::getInstance().respondMessage(jsonMessage);

					break;
				}
			}
		}
	}

	// Server confirm player withdrawing
	else if (jsonMessage["MessageType"] == "MERCHANT_WITHDRAW_CARDS_RESPONSE") {
		// Check if it is user's withdraw
		if (jsonMessage["PlayerName"] == m_playerList[USER_PLAYER_INDEX]->getPlayerName()) {
			if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
				// Add new card to user hand and start animation
				addToUserHand(Card::m_stringToCardName.at(jsonMessage["Card"]));
				m_gameLogic->handleWithdrawEvent(Game::PileType::LEFT_DISCARD_PILE);
			}
			else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
				// Add new card to user hand and start animation
				addToUserHand(Card::m_stringToCardName.at(jsonMessage["Card"]));
				m_gameLogic->handleWithdrawEvent(Game::PileType::RIGHT_DISCARD_PILE);
			}
			else if (jsonMessage["Pile"] == "MAIN_DECK") {
				// Add new card to user hand and start animation
				addToUserHand(Card::m_stringToCardName.at(jsonMessage["Card"]));
				m_gameLogic->handleWithdrawEvent(Game::PileType::MAIN_DECK);
			}
		}
		
		// If it is not user's withdraw, setup animation of opponent's withdraw
		else {
			for (int i = 1; i < m_playerList.size(); i++) {
				if (jsonMessage["PlayerName"] == m_playerList[i]->getPlayerName()) {
					if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
						m_gameLogic->handleOpponentWithdrawEvent(Game::PileType::LEFT_DISCARD_PILE, i);
					}
					else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
						m_gameLogic->handleOpponentWithdrawEvent(Game::PileType::RIGHT_DISCARD_PILE, i);
					}
					else if (jsonMessage["Pile"] == "MAIN_DECK") {
						m_gameLogic->handleOpponentWithdrawEvent(Game::PileType::MAIN_DECK, i);
					}
					break;
				}
			}
		}

		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server confirm player discarding
	else if (jsonMessage["MessageType"] == "MERCHANT_DISCARD_CARDS_RESPONSE") {
		// Check if it is user's discard
		if (jsonMessage["PlayerName"] == m_playerList[USER_PLAYER_INDEX]->getPlayerName()) {
			std::string cardName = jsonMessage["Card"];
			if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
				m_gameLogic->handleDiscardEvent(Game::PileType::LEFT_DISCARD_PILE, cardName);
			}
			else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
				m_gameLogic->handleDiscardEvent(Game::PileType::RIGHT_DISCARD_PILE, cardName);
			}
			setupPlayerUI();
		}

		// If it is not user's discard, setup animation of opponent's discard
		else {
			for (int i = 1; i < m_playerList.size(); i++) {
				if (jsonMessage["PlayerName"] == m_playerList[i]->getPlayerName()) {
					// Get card type from message
					Card::CardType card = Card::m_stringToCardName.at(jsonMessage["Card"]);
					if (jsonMessage["Pile"] == "LEFT_DISCARD_PILE") {
						m_gameLogic->handleOpponentDiscardEvent(Game::PileType::LEFT_DISCARD_PILE, i, card);
					}
					else if (jsonMessage["Pile"] == "RIGHT_DISCARD_PILE") {
						m_gameLogic->handleOpponentDiscardEvent(Game::PileType::RIGHT_DISCARD_PILE, i, card);
					}
				}
			}
		}

		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server notify Sheriff Check
	else if (jsonMessage["MessageType"] == "SHERIFF_CHECK_RESPONSE") {
		// Set game state
		m_gameEvent = REVEAL;
		// Set info text
		m_textMutex.lock();
		m_infoText.setScale(1.f, 1.f);
		m_infoText.setString("Sheriff chose to inspect the goods!");
		m_infoText.setPosition((1920.f - m_infoText.getGlobalBounds().width) / 2, 680.f);
		m_textMutex.unlock();
		// Handle animations
		m_gameLogic->handleSheriffInspectEvent(jsonMessage);
	}

	// Server notify Sheriff Check
	else if (jsonMessage["MessageType"] == "SHERIFF_PASS_RESPONSE") {
		// Set game state
		m_gameEvent = REVEAL;
		// If the timer is running, turn it off
		m_timer->stop();
		// Set info text
		m_textMutex.lock();
		m_infoText.setString("Sheriff chose to let the goods pass!");
		m_infoText.setScale(1.f, 1.f);
		m_infoText.setPosition((1920.f - m_infoText.getGlobalBounds().width) / 2, 680.f);
		m_textMutex.unlock();
		// Handle animations
		m_gameLogic->handleSheriffPassEvent(jsonMessage);
	}

	// Server notify timeout
	else if (jsonMessage["MessageType"] == "PLAYER_TIMEOUT" && jsonMessage["PlayerName"] == m_playerList[USER_PLAYER_INDEX]->getPlayerName()) {
		// Stop the timer
		m_timer->stop();

		//// If player is in the middle of withdraw/discard event, randomize what's left
		//if (m_gameEvent == WITHDRAW) {
		//	m_gameLogic->handleTimeoutWithdraw();
		//}
		//else if (m_gameEvent == DISCARD) {
		//	m_gameLogic->handleTimeoutDiscard();
		//}
		//// If not, send response message and wait for new turn
		//else {
		if (m_gameEvent == DISCARD) {
			m_gameEvent = TIMEOUT_DISCARD;
		}
		//}
		// Respond message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Server update player score
	else if (jsonMessage["MessageType"] == "PLAYER_UPDATE_INFO") {
		for (auto& player : m_playerList) {
			if (player->getPlayerName() == jsonMessage["PlayerName"]) {
				std::unordered_map<Card::CardType, int> playerCardMap;
				std::unordered_map<Card::CardType, int> blackMarketBonusMap;
				int playerMoney = player->getPlayerMoney();
				int playerScore = player->getPlayerScore();

				// Get money and score info
				if (jsonMessage.contains("Money")) {
					std::string money = jsonMessage["Money"];
					playerMoney = std::stoi(money);
				}
				if (jsonMessage.contains("Point")) {
					std::string score = jsonMessage["Point"];
					playerScore = std::stoi(score);
				}
				
				// Get cards info
				if (jsonMessage.contains("Cards") && jsonMessage["Cards"].is_object()) {
					for (auto it = jsonMessage["Cards"].begin(); it != jsonMessage["Cards"].end(); it++) {
						std::string cardTypeString = it.key();
						Card::CardType cardType = Card::m_stringToCardName.at(cardTypeString);
						std::string cardCountString = it.value();
						int cardCount = std::stoi(cardCountString);
						// Push to map
						playerCardMap[cardType] = cardCount;
					}
				}

				// Get black market bonuses info
				if (jsonMessage.contains("BlackMarketBonus") && jsonMessage["BlackMarketBonus"].is_object()) {
					for (auto it = jsonMessage["BlackMarketBonus"].begin(); it != jsonMessage["BlackMarketBonus"].end(); it++) {
						std::string cardTypeString = it.key();
						Card::CardType cardType = Card::m_stringToCardName.at(cardTypeString);
						std::string bonusCountString = it.value();
						int bonusCount = std::stoi(bonusCountString);
						// Push to map
						blackMarketBonusMap[cardType] = bonusCount;
					}
				}

				m_gameLogic->updatePlayerInfo(player, playerMoney, playerScore, playerCardMap, blackMarketBonusMap);

				break;
			}
		}
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Game end
	else if (jsonMessage["MessageType"] == "GAME_END") {
		g_gameState = END_VIEW;
		Network::getInstance().respondMessage(jsonMessage);
	}

}




