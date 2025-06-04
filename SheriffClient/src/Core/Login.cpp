#include "../include/Core/Login.h"
#include "../include/Core/GameState.h"

#define CLIENT_VER			"v0.8.3"

bool Login::initVariables()
{
	m_window = nullptr;
	m_mousePressedPos = INVALID;

	return true;
}

bool Login::initWindow()
{
	//m_videoMode.width = 1920;
	//m_videoMode.height = 1080;
	//m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window = setupScaledWindow();
	m_window->setVerticalSyncEnabled(true);

	m_popup = new Popup(m_window, 500, 200);

	// Create UI for login screen
	// Font
	if (!m_font.loadFromFile("assets/arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	// Background
	if (!m_backgroundTexture.loadFromFile("assets/Images/LoginBackground.png")) {
		std::cerr << "Error loading background texture!\n";
	}

	// Shader
	if (!m_glowShader.loadFromFile("assets/Shaders/glow.frag", sf::Shader::Fragment)) {
		std::cerr << "Error loading shader!\n";
	}

	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setFillColor(sf::Color::White);
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));
	m_background.setTexture(&m_backgroundTexture);

	// Client Version
	m_clientVersion = sf::Text(CLIENT_VER, m_font, 32);
	m_clientVersion.setFillColor(sf::Color::White);
	m_clientVersion.setOutlineColor(sf::Color::Black);
	m_clientVersion.setOutlineThickness(1.f);
	m_clientVersion.setPosition(40.f, 1000.f);

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

	// User name display text
	m_usernameDisplay.setFont(m_font);
	m_usernameDisplay.setFillColor(sf::Color::Black);

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

	// Init shaders
	//m_glowShader.setUniform("glowColor", sf::Glsl::Vec4(1.0, 1.0, 1.0, 1.0));
	//m_glowShader.setUniform("glowIntensity", 0.0f); // Default no glow
	m_glowShader.setUniform("isHovered", false);
	m_glowShader.setUniform("outlineColor", sf::Glsl::Vec4(1.0, 1.0, 1.0, 1.0));
	m_glowShader.setUniform("glowStrength", 0.35f); // Adjust glow thickness

	return true;
}

sf::RenderWindow* Login::setupScaledWindow(const std::string & title)
{
	const float GAME_WIDTH = 1920.f;
	const float GAME_HEIGHT = 1080.f;

	// Get desktop resolution
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	float screenWidth = static_cast<float>(desktop.width);
	float screenHeight = static_cast<float>(desktop.height);

	float scale_height;
	float scale_width;
	float scale_final;

	scale_width = screenWidth / GAME_WIDTH;
	scale_height = screenHeight / GAME_HEIGHT;
	if (scale_width >= 1 && scale_height >= 1) {
		scale_final = 1;
	}
	else {
		scale_final = (scale_width < scale_height) ? (scale_width) : (scale_height);
	}

	// Final window size (fitting screen while preserving aspect ratio)
	unsigned int windowWidth = static_cast<unsigned int>(GAME_WIDTH * scale_final);
	unsigned int windowHeight = static_cast<unsigned int>(GAME_HEIGHT * scale_final);

	// Create the window
	sf::RenderWindow* window = new sf::RenderWindow(
		sf::VideoMode(windowWidth, windowHeight),
		title,
		sf::Style::Default
	);

	// Set view to maintain logical 1920x1080 coordinates
	sf::View view(sf::FloatRect(0.f, 0.f, GAME_WIDTH, GAME_HEIGHT));

	window->setView(view);

	return window;
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
		sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_window);
		sf::Vector2f mousePosXY = m_window->mapPixelToCoords(mousePosXYLocal);
		switch (m_ev.type) {
		// Case close window
		case sf::Event::Closed:
			m_window->close();
			g_gameState = gameState::INVALID;

			break;

		// Case hover
		case sf::Event::MouseMoved:
			if (m_loginButton.getGlobalBounds().contains(mousePosXY)) {
				m_glowShader.setUniform("isHovered", true);
			}
			else {
				m_glowShader.setUniform("isHovered", false);
			}

			break;

		// Case left click
		case sf::Event::MouseButtonPressed:
			m_mousePressedPos = INVALID;
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				if (m_loginButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);
					m_mousePressedPos = LOGIN_BUTTON;
				}
				else if (m_usernameBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);
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
					//m_usernameText += static_cast<char>(m_ev.text.unicode);
					std::string temp = m_usernameText + static_cast<char>(m_ev.text.unicode);
					m_usernameDisplay.setString(temp);

					// Measure the width
					if (m_usernameDisplay.getLocalBounds().width <= 320) {
						m_usernameText += static_cast<char>(m_ev.text.unicode);
					}
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
	m_usernameDisplay = sf::Text(m_usernameText, m_font, 30);
	m_usernameDisplay.setFillColor(sf::Color::Black);
	m_usernameDisplay.setPosition(m_usernameBox.getPosition().x + 10, m_usernameBox.getPosition().y + 5);

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
				m_popup->setMessage("Failed to connect to the server.\n");
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

	m_window->draw(m_loginButton, &m_glowShader);
	m_window->draw(m_loginButtonText);

	// Render client version
	m_window->draw(m_clientVersion);

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
		// Handle reject reasons
		std::string reason = jsonMessage["Reason"];
		if (reason == "FULL") {
			m_popup->setMessage("Lobby is full.\n");
		}
		else if (reason == "SAME_NAME_EXISTS") {
			m_popup->setMessage("Someone picked your name.\n");
		}
		else if (reason == "GAME_STARTED") {
			m_popup->setMessage("Game started.");
		}
		else {
			m_popup->setMessage("Server refused your request. We don't know why :(");
		}
		
		m_popup->show();

		std::cout << "Server refused to accept.\n";

		// Send a response message
		Network::getInstance().respondMessage(jsonMessage);
	}

	// Handle Login success (in this case, response message is not sent here, but sent after Lobby is initiated)
	else if (jsonMessage["MessageType"] == "GAME_ACCEPT_PLAYER") {
		// Move on to Lobby screen
		m_usernameText = jsonMessage["PlayerName"];
		Network::getInstance().setUserPlayerName(m_usernameText);
		g_gameState = LOBBY_VIEW;
	}

	else {
		std::cout << "Message received but could not decode meaningful data.\n";
	}

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
