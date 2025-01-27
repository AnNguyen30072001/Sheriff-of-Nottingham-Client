#include "login.h"
#include "GameState.h"

bool Login::initVariables()
{
	m_window = nullptr;
	m_mousePressedPos = INVALID;

	return true;
}

bool Login::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	m_popup = new Popup(m_window);

	// Create UI for login screen
	// Font
	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
		return false;
	}

	// Background
	if (!m_backgroundTexture.loadFromFile("Images/LoginBackground.png")) {
		std::cerr << "Error loading background texture!";
		return false;
	}
	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setFillColor(sf::Color::White);
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));
	m_background.setTexture(&m_backgroundTexture);

	// Username Label
	m_usernameLabel = sf::Text("Username:", m_font, 24);
	m_usernameLabel.setFillColor(sf::Color::White);
	m_usernameLabel.setOutlineColor(sf::Color::Black);
	m_usernameLabel.setOutlineThickness(1.f);
	m_usernameLabel.setPosition(700, 560);

	// IP Label
	//m_IPLabel = sf::Text("IP Address:", m_font, 24);
	//m_IPLabel.setFillColor(sf::Color::White);
	//m_IPLabel.setOutlineColor(sf::Color::Black);
	//m_IPLabel.setOutlineThickness(1.f);
	//m_IPLabel.setPosition(700, 660);

	// Username Input Box
	m_usernameBox = sf::RectangleShape(sf::Vector2f(400, 50));
	m_usernameBox.setFillColor(sf::Color(200, 200, 200));
	m_usernameBox.setPosition(850, 550);

	// IP Input Box
	//m_IPBox = sf::RectangleShape(sf::Vector2f(400, 50));
	//m_IPBox.setFillColor(sf::Color(200, 200, 200));
	//m_IPBox.setPosition(850, 650);

	// Box outline
	m_BoxOutline = sf::RectangleShape(sf::Vector2f(400, 50));
	m_BoxOutline.setFillColor(sf::Color::White);

	// Login Button
	m_loginButton = sf::RectangleShape(sf::Vector2f(200, 50));
	m_loginButton.setFillColor(sf::Color::Blue);
	m_loginButton.setPosition((1920 - 200) / 2, 650);

	// Login Button Text
	m_loginButtonText = sf::Text("Login", m_font, 24);
	m_loginButtonText.setFillColor(sf::Color::White);
	m_loginButtonText.setPosition(
		m_loginButton.getPosition().x + (m_loginButton.getSize().x - m_loginButtonText.getLocalBounds().width) / 2,
		m_loginButton.getPosition().y + (m_loginButton.getSize().y - m_loginButtonText.getLocalBounds().height) / 2 - 5
	);

	return true;
}

Login::Login()
{
	initVariables();
	initWindow();
}

Login::~Login()
{
	delete m_window;
}

const bool Login::running() const
{
	return m_window->isOpen();
}

std::string Login::getUsername() const
{
	return m_usernameText;
}

bool Login::pollEvents()
{
	while (m_window->pollEvent(m_ev)) {
		switch (m_ev.type) {
		// Case close window
		case sf::Event::Closed:
			m_window->close();
			break;

		// Case left click
		case sf::Event::MouseButtonPressed:
			m_mousePressedPos = INVALID;
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_window);
				sf::Vector2f mousePosXY = m_window->mapPixelToCoords(mousePosXYLocal);
				if (m_loginButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					m_mousePressedPos = LOGIN_BUTTON;
				}
				else if (m_usernameBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					m_mousePressedPos = USERNAME_BOX;
					m_BoxOutline.setPosition(m_usernameBox.getPosition());
				}
				//else if (m_IPBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
				//	m_mousePressedPos = IP_BOX;
				//	m_BoxOutline.setPosition(m_IPBox.getPosition());
				//}
				else {

				}
			}
			break;
		
		// Case enter login info
		case sf::Event::TextEntered:
			if (m_mousePressedPos == USERNAME_BOX) {
				if (m_ev.text.unicode == '\b' && !m_usernameText.empty()) { // Backspace
					m_usernameText.pop_back();
				}
				else if (m_ev.text.unicode >= 32 && m_ev.text.unicode <= 126) { // Printable characters
					m_usernameText += static_cast<char>(m_ev.text.unicode);
				}
			}
			//else if (m_mousePressedPos == IP_BOX) {
			//	if (m_ev.text.unicode == '\b' && !m_IPText.empty()) { // Backspace
			//		m_IPText.pop_back();
			//	}
			//	else if (m_ev.text.unicode >= 32 && m_ev.text.unicode <= 126) { // Printable characters
			//		m_IPText += static_cast<char>(m_ev.text.unicode);
			//	}
			//}
			break;
		default:
			// Do nothing
			break;
		}
	}

	return true;
}

bool Login::update()
{
	m_popup->pollEvents();
	if (!m_popup->isVisible()) {
		pollEvents();
	}

	// Username text display
	m_usernameDisplay = sf::Text(m_usernameText, m_font, 24);
	m_usernameDisplay.setFillColor(sf::Color::Black);
	m_usernameDisplay.setPosition(m_usernameBox.getPosition().x + 10, m_usernameBox.getPosition().y + 10);

	// IP Address display
	//m_IPDisplay = sf::Text(m_IPText, m_font, 24);
	//m_IPDisplay.setFillColor(sf::Color::Black);
	//m_IPDisplay.setPosition(m_IPBox.getPosition().x + 10, m_IPBox.getPosition().y + 10);

	if (m_mousePressedPos == LOGIN_BUTTON) {
		m_mousePressedPos = INVALID;

		if (m_usernameText != "") {
			// Attempt connection to server
			if (!Network::getInstance().connect()) {
				// Show Popup saying connection failed
				m_popup->setMessage("Failed to connect to the server.\nYou have Cambodian Wifi or what?");
				m_popup->show();

				std::cerr << "Failed to connect to the server." << std::endl;
			}

			// Connection success. Now request Server for participation.
			else {
				Network::getInstance().startListening();
				Network::getInstance().startProcessingMessageQueue();

				std::cout << "Connected to the server." << std::endl;

				// Log
				std::cout << "Login infomation sending to server...\n";
				std::cout << "Username: " << m_usernameText << "\n";

				// Prepare JSON package
				json message;
				message["MessageType"] = "PLAYER_JOIN";
				message["PlayerName"] = m_usernameText;
				// Encode to string type
				std::string messageString = message.dump();
				// Send to server
				sendMessageToServer(messageString);
			}
		}
	}
	return true;
}

bool Login::render()
{
	// Clear screen to black screen
	m_window->clear();

	// Draw login screen
	m_window->draw(m_background);
	m_window->draw(m_usernameBox);
	m_window->draw(m_usernameLabel);

	//m_window->draw(m_IPBox);
	//m_window->draw(m_IPLabel);

	if (m_mousePressedPos == USERNAME_BOX || m_mousePressedPos == IP_BOX) {
		m_window->draw(m_BoxOutline);
	}

	m_window->draw(m_usernameDisplay);
	//m_window->draw(m_IPDisplay);

	m_window->draw(m_loginButton);
	m_window->draw(m_loginButtonText);

	// Render Popup if showed
	m_popup->render();

	// Display
	m_window->display();
	
	return true;
}

void Login::onMessageReceived(const nlohmann::json& jsonMessage)
{
	// Log
	std::cout << "Login received from Server: " << jsonMessage << std::endl;

	// Handle message type
	if (jsonMessage["MessageType"] == "GAME_REJECT_PLAYER") {
		m_popup->setMessage("Server refused to accept. Guess why mf.");
		m_popup->show();

		std::cout << "Server refused to accept. Guess why mf.\n";
	}

	// Handle Login success
	else if (jsonMessage["MessageType"] == "GAME_ACCEPT_PLAYER") {
		// Move on to Lobby screen
		m_usernameText = jsonMessage["PlayerName"];
		g_gameState = LOBBY_VIEW;
	}

	else {
		std::cout << "Message received but could not decode meaningful data.\n";
	}

	// Send a response message
	Network::getInstance().respondMessage(jsonMessage);
}

void Login::sendMessageToServer(const std::string & message)
{
	if (Network::sendMessage(message)) {
		std::cout << "JSON message sent successfully.\n";
	}
	else {
		std::cerr << "Failed to send JSON message.\n";
	}
}
