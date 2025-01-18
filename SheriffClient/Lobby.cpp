#include <iostream>
#include "lobby.h"

bool Lobby::initVariable()
{
	m_window = nullptr;
	if (!m_backgroundTexture.loadFromFile("Images/Background.png")) {
		std::cerr << "Error loading background texture!";
		return false;
	}
	m_backgroundTexture.setSmooth(true);

	return true;
}

bool Lobby::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setFillColor(sf::Color::White);
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));
	m_background.setTexture(&m_backgroundTexture);

	return true;
}

Lobby::Lobby()
{
	initVariable();
	initWindow();
}

Lobby::~Lobby()
{
	delete this->m_window;
	for (int i = 0; i < m_playerList.size(); i++) {
		delete m_playerList[i];
	}
}

const bool Lobby::running() const
{
	return m_window->isOpen();
}

bool Lobby::addToPlayerList(std::string name, sf::Color playerColor, bool isUserPlayer)
{
	m_playerList.push_back(new Player(name, playerColor, isUserPlayer));

	return true;
}

bool Lobby::removeFromPlayerList(std::string name)
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->getPlayerName() == name) {
			m_playerList.erase(m_playerList.begin() + i);
			return true;
		}
	}

	return false;
}

bool Lobby::setupPlayerUI()
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

bool Lobby::handleMouseClick(sf::Vector2i mousePosXY)
{
	for (auto& player : m_playerList) {
		if (player->isUserPlayer()) {
			if (player->getReadyButton().getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
				// Need to send status of user to Server 
				bool current_status = player->isPlayerReady();
				player->setPlayerReady(!current_status);
				player->setReadyButtonAppearance(player->isPlayerReady() ?
					sf::Color::Green : sf::Color(128, 128, 128));
			}
		}
	}

	return true;
}

bool Lobby::pollEvents()
{
	while (m_window->pollEvent(m_ev)) {
		switch (m_ev.type) {
		case sf::Event::Closed:
			m_window->close();
			break;
		case sf::Event::MouseButtonPressed:
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXY = sf::Mouse::getPosition(*m_window);
				handleMouseClick(mousePosXY);
			}
			break;
		default:
			break;
		}
	}

	return true;
}

bool Lobby::update()
{
	setupPlayerUI();
	pollEvents();

	return true;
}

bool Lobby::render()
{
	m_window->clear();

	m_window->draw(m_background);
	for (const auto& player : m_playerList) {
		m_window->draw(player->getAvatar());	
		m_window->draw(player->getReadyButton());
		m_window->draw(player->getReadyText());
		m_window->draw(player->getNameText());
	}

	m_window->display();

	return true;
}
