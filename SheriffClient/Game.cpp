#include "game.h"

void Game::initVariables(std::vector<Player*> playerList)
{
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

	deck = new Deck();
	deck->setDiscardDeckLeftTexture(Card::APPLE);
	deck->setDiscardDeckRightTexture(Card::CROSSBOW);


}

void Game::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setFillColor(sf::Color::White);
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));
	m_background.setTexture(&m_backgroundTexture);

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
	delete this->m_window;
}

const bool Game::running() const
{
	return m_window->isOpen();
}

bool Game::addToUserHand(Card::CardType card)
{
	m_userHand.push_back(new Card(card));

	return true;
}

bool Game::removeFromUserHand(Card::CardType card)
{
	for (int i = 0; i < m_userHand.size(); i++) {
		if (m_userHand[i]->getCardType() == card) {
			m_userHand.erase(m_userHand.begin() + i);
			return true;
		}
	}
}

bool Game::handleMouseClick(sf::Vector2f mousePosXY)
{
	for (auto& card : m_userHand) {
		if (card->getCard().getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			card->setSelected(!card->isSelected());
		}
	}

	// If any card is selected and it is user's turn, the left and right buttons are interactable
	if (m_anyCardSelected && m_playerList[0]->isInTurn()) {
		if (m_ButtonLeft.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			// Handle discard/inspect
			std::cout << "Discard/Inspect\n";
		}

		if (m_ButtonRight.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
			// Handle present/pass
			std::cout << "Present/Pass\n";
		}
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
	setupPlayerUI();
	updateUserHandUI();
	pollEvents();

	return true;
}

bool Game::render()
{
	m_window->clear();

	// Background
	m_window->draw(m_background);

	// Decks
	m_window->draw(deck->getMainDeck());
	m_window->draw(deck->getDiscardDeckLeft());
	m_window->draw(deck->getDiscardDeckRight());

	for (const auto& player : m_playerList) {
		m_window->draw(player->getAvatar());
		m_window->draw(player->getNameText());
		m_window->draw(player->getInfoTabIcon());
		m_window->draw(player->getSheriffBadge());
		m_window->draw(player->getTurnIndicator());
	}

	for (const auto& card : m_userHand) {
		m_window->draw(card->getCard());
	}

	// Buttons
	m_window->draw(m_ButtonLeft);
	m_window->draw(m_ButtonLeftText);
	m_window->draw(m_ButtonRight);
	m_window->draw(m_ButtonRightText);

	m_window->display();

	return true;
}

bool Game::setupPlayerUI()
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->isUserPlayer()) {
			continue;
		}
		// Positioning
		float xOffset = -230.f + (i % 6) * 380.f; // Distribute evenly horizontally
		//float yOffset = (i < 3) ? 200.f : 400.f; // Top or bottom row

		m_playerList[i]->initPlayer(xOffset, 50.f);
	}

	return true;
}

bool Game::updateUserHandUI()
{
	m_anyCardSelected = false;
	for (int i = 0; i < m_userHand.size(); i++) {
		float posX = 520.f + (i % 6) * 150.f;
		if (m_userHand[i]->isSelected()) {
			m_userHand[i]->setupCardUI(posX, 600.f);
			m_anyCardSelected = true;
		}
		else {
			m_userHand[i]->setupCardUI(posX, 635.f);
		}
	}

	return true;
}

void Game::onMessageReceived(const nlohmann::json& jsonMessage)
{
	std::cout << "Game receive from Server: " << jsonMessage << std::endl;
}
