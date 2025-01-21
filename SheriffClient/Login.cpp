#include "login.h"

bool Login::initVariables()
{
	m_window = nullptr;
	m_mousePressedPos = INVALID;
	/*m_availableColor = {
		sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
	};
	m_colorMap[sf::Color::Red] = "Red";
	m_colorMap[sf::Color::Green] = "Green";
	m_colorMap[sf::Color::Blue] = "Blue";
	m_colorMap[sf::Color::Yellow] = "Yellow";
	m_colorMap[sf::Color::Magenta] = "Magenta";
	m_colorMap[sf::Color::Cyan] = "Cyan";*/

	return true;
}

bool Login::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

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

	// m_title Text
	/*m_title = sf::Text("Welcome to Nottingham!", m_font, 48);
	m_title.setFillColor(sf::Color::White);
	m_title.setPosition((1920 - m_title.getLocalBounds().width) / 2, 250);*/

	// Username Label
	m_usernameLabel = sf::Text("Username:", m_font, 24);
	m_usernameLabel.setFillColor(sf::Color::White);
	m_usernameLabel.setOutlineColor(sf::Color::Black);
	m_usernameLabel.setOutlineThickness(1.f);
	m_usernameLabel.setPosition(700, 560);

	// IP Label
	m_IPLabel = sf::Text("IP Address:", m_font, 24);
	m_IPLabel.setFillColor(sf::Color::White);
	m_IPLabel.setOutlineColor(sf::Color::Black);
	m_IPLabel.setOutlineThickness(1.f);
	m_IPLabel.setPosition(700, 660);

	// Username Input Box
	m_usernameBox = sf::RectangleShape(sf::Vector2f(400, 50));
	m_usernameBox.setFillColor(sf::Color(200, 200, 200));
	m_usernameBox.setPosition(850, 550);

	// IP Input Box
	m_IPBox = sf::RectangleShape(sf::Vector2f(400, 50));
	m_IPBox.setFillColor(sf::Color(200, 200, 200));
	m_IPBox.setPosition(850, 650);

	// Box outline
	m_BoxOutline = sf::RectangleShape(sf::Vector2f(400, 50));
	m_BoxOutline.setFillColor(sf::Color::White);

	// Login Button
	m_loginButton = sf::RectangleShape(sf::Vector2f(200, 50));
	m_loginButton.setFillColor(sf::Color::Blue);
	m_loginButton.setPosition((1920 - 200) / 2, 750);

	// Login Button Text
	m_loginButtonText = sf::Text("Login", m_font, 24);
	m_loginButtonText.setFillColor(sf::Color::White);
	m_loginButtonText.setPosition(
		m_loginButton.getPosition().x + (m_loginButton.getSize().x - m_loginButtonText.getLocalBounds().width) / 2,
		m_loginButton.getPosition().y + (m_loginButton.getSize().y - m_loginButtonText.getLocalBounds().height) / 2 - 5
	);

	//// Create color boxes
	//float colorBoxSize = 50; // Size of each color box
	//float spacing = 10; // Spacing between boxes

	//for (size_t i = 0; i < m_availableColor.size(); i++) {
	//	sf::RectangleShape colorBox(sf::Vector2f(colorBoxSize, colorBoxSize));
	//	colorBox.setFillColor(m_availableColor[i]);
	//	colorBox.setPosition(850 + i * (colorBoxSize + spacing), 600);
	//	m_colorOptions.push_back(colorBox);
	//}

	//// Initialize selected color highlight
	//m_selectedColorOutline = sf::RectangleShape(sf::Vector2f(colorBoxSize, colorBoxSize));
	//m_selectedColorOutline.setFillColor(sf::Color::Transparent);
	//m_selectedColorOutline.setOutlineColor(sf::Color::White);
	//m_selectedColorOutline.setOutlineThickness(2);
	//m_selectedColorOutline.setPosition(m_colorOptions[0].getPosition());
	//m_selectedColor = m_availableColor[0]; // Default selected color

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
				else if (m_IPBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					m_mousePressedPos = IP_BOX;
					m_BoxOutline.setPosition(m_IPBox.getPosition());
				}
				else {
					//for (size_t i = 0; i < m_colorOptions.size(); i++) {
					//	if (m_colorOptions[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					//		// Store selected color and outline
					//		m_selectedColor = m_colorOptions[i].getFillColor();
					//		m_selectedColorOutline.setPosition(m_colorOptions[i].getPosition());
					//	}
					//}
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
			else if (m_mousePressedPos == IP_BOX) {
				if (m_ev.text.unicode == '\b' && !m_IPText.empty()) { // Backspace
					m_IPText.pop_back();
				}
				else if (m_ev.text.unicode >= 32 && m_ev.text.unicode <= 126) { // Printable characters
					m_IPText += static_cast<char>(m_ev.text.unicode);
				}
			}
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
	pollEvents();

	// Username text display
	m_usernameDisplay = sf::Text(m_usernameText, m_font, 24);
	m_usernameDisplay.setFillColor(sf::Color::Black);
	m_usernameDisplay.setPosition(m_usernameBox.getPosition().x + 10, m_usernameBox.getPosition().y + 10);

	// IP Address display
	m_IPDisplay = sf::Text(m_IPText, m_font, 24);
	m_IPDisplay.setFillColor(sf::Color::Black);
	m_IPDisplay.setPosition(m_IPBox.getPosition().x + 10, m_IPBox.getPosition().y + 10);

	if (m_mousePressedPos == LOGIN_BUTTON) {
		m_mousePressedPos = INVALID;
		// Send infomation to server
		std::cout << "Login infomation sending to server...\n";
		std::cout << "Username: " << m_usernameText << "\nIP Address: " << m_IPText << "\n";
		std::string message = "Username:" + m_usernameText + ".IP:" + m_IPText;
		sendMessageToServer(message);
		//std::cout << "Selected color: " << m_colorMap[m_selectedColor] << "\n";
	}
	return true;
}

bool Login::render()
{
	// Clear screen to black screen
	m_window->clear();

	// Draw login screen
	m_window->draw(m_background);
	//m_window->draw(m_title);
	m_window->draw(m_usernameBox);
	m_window->draw(m_usernameLabel);

	m_window->draw(m_IPBox);
	m_window->draw(m_IPLabel);

	/*for (const auto& colorBox : m_colorOptions) {
		m_window->draw(colorBox);
	}
	m_window->draw(m_selectedColorOutline);*/

	if (m_mousePressedPos == USERNAME_BOX || m_mousePressedPos == IP_BOX) {
		m_window->draw(m_BoxOutline);
	}

	m_window->draw(m_usernameDisplay);
	m_window->draw(m_IPDisplay);

	m_window->draw(m_loginButton);
	m_window->draw(m_loginButtonText);

	

	// Display
	m_window->display();
	
	return true;
}

void Login::onMessageReceived(const std::string & msg)
{
	std::cout << "Login received from Server: " << msg << std::endl;
}

void Login::sendMessageToServer(const std::string & message)
{
	Network::sendMessage(message);
}
