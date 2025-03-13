#include <iostream>
#include "../include/Core/Lobby.h"
#include "../include/Core/GameState.h"
#include <Windows.h>

bool Lobby::initVariable()
{
	m_newPlayerIdx = 1;
	m_window = nullptr;
	if (!m_backgroundTexture.loadFromFile("assets/Images/Background_demo.png")) {
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
}

const bool Lobby::running() const
{
	return m_window->isOpen();
}

std::vector<Player*> Lobby::getPlayerList() const
{
	return m_playerList;
}

bool Lobby::addToPlayerList(std::string name, sf::Color playerColor, bool isUserPlayer, Lobby::insertPos insertPos)
{
	std::lock_guard<std::mutex> lock(m_PlayerMutex);
	if (insertPos == TAIL) {
		m_playerList.push_back(new Player(name, playerColor, isUserPlayer));
	}
	else {
		m_playerList.insert(m_playerList.begin() + m_newPlayerIdx, new Player(name, playerColor, isUserPlayer));
		m_newPlayerIdx++;
	}

	return true;
}

bool Lobby::removeFromPlayerList(std::string name)
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->getPlayerName() == name) {
			std::lock_guard<std::mutex> lock(m_PlayerMutex);
			m_playerList.erase(m_playerList.begin() + i);
			return true;
		}
	}

	return false;
}

bool Lobby::setupPlayerUI()
{
	std::lock_guard<std::mutex> lock(m_PlayerMutex);
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->isUserPlayer()) {
			continue;
		}
		// Positioning
		float xOffset = -230.f + (i % 6) * 380.f; // Distribute evenly horizontally

		m_playerList[i]->initPlayer(xOffset, 50.f);
	}

	return true;
}

bool Lobby::handleMouseClick(sf::Vector2f mousePosXY)
{
	for (auto& player : m_playerList) {
		if (player->isUserPlayer()) {
			if (player->getReadyButton().getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
				// Play a sound
				m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

				// Status
				bool prev_status = player->isPlayerReady();

				// Need to send status of user to Server 
				json message;
				message["MessageType"] = prev_status ? "PLAYER_UNREADY" : "PLAYER_READY";
				message["PlayerName"] = player->getPlayerName();

				std::string messageString = message.dump();
				Network::sendMessage(messageString);
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
	std::lock_guard<std::mutex> lock(m_PlayerMutex);
	for (const auto& player : m_playerList) {
		m_window->draw(player->getAvatar());	
		m_window->draw(player->getAvatarFrame());	
		m_window->draw(player->getReadyButton());
		m_window->draw(player->getReadyText());
		m_window->draw(player->getNameText());
	}

	m_window->display();

	return true;
}

void Lobby::onMessageReceived(const nlohmann::json& jsonMessage)
{
	// Log
	std::cout << "Lobby receive from Server: " << jsonMessage << std::endl;

	// Handle previously joined players
	if (jsonMessage["MessageType"] == "GAME_CONNECTED_PLAYER_RECENT") {
		// Retrieve all player names
		std::vector<std::string> playerNames;

		// Get all previously joined players' name
		for (auto it = jsonMessage["PlayersName"].begin(); it != jsonMessage["PlayersName"].end(); it++) {
			playerNames.push_back(it.value().get<std::string>());
		}

		// Append to player list
		for (int i = 0; i < playerNames.size(); i++) {
			this->addToPlayerList(playerNames[i], sf::Color::Blue, false);
			//if (jsonMessage.contains("PlayersState")) {
			//	bool isReady = jsonMessage["PlayersState"][i].get<std::string>() == "PLAYER_READY" ? true : false;
			//	m_playerList[m_playerList.size() - 1]->setPlayerReady(isReady);
			//}
		}
	}

	// Handle a newly joined player
	else if (jsonMessage["MessageType"] == "GAME_CONNECTED_PLAYER_NOW") {
		std::string playerName = jsonMessage["PlayerName"];
		this->addToPlayerList(playerName, sf::Color::Blue, false, Lobby::FRONT);
	}

	// Handle disconnected players
	else if (jsonMessage["MessageType"] == "GAME_DISCONNECT_PLAYER") {
		std::string playerName = jsonMessage["PlayerName"];
		this->removeFromPlayerList(playerName);
	}

	// Handle player ready status
	else if (jsonMessage["MessageType"] == "GAME_ACCEPT_READY") {
		std::string playerName = jsonMessage["PlayerName"];
		for (auto& player : m_playerList) {
			if (player->getPlayerName() == playerName) {
				player->setPlayerReady(true);
				player->setReadyButtonAppearance(sf::Color::Green);
				break;
			}
		}
	}

	// Handle player unready status
	else if (jsonMessage["MessageType"] == "GAME_ACCEPT_UNREADY") {
		for (auto& player : m_playerList) {
			if (player->getPlayerName() == jsonMessage["PlayerName"]) {
				player->setPlayerReady(false);
				player->setReadyButtonAppearance(sf::Color(128, 128, 128));
				break;
			}
		}
	}

	// Handle game start (in this case, response message is not sent here, but sent after Lobby is initiated)
	else if (jsonMessage["MessageType"] == "GAME_START") {
		// TO DO: Add UI to indicate game about to start 
		Sleep(1000);
		g_gameState = GAME_VIEW;
	}

	else {
		std::cout << "Message received but could not decode meaningful data.\n";
	}

	// Send response message if not GAME_START event
	if (jsonMessage["MessageType"] != "GAME_START") {
		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

}
