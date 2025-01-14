#include "login.h"

bool Login::initVariables()
{
	this->m_window = nullptr;
	this->m_mousePressedPos = INVALID;
	this->m_availableColor = {
		sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
	};
	this->m_colorMap[sf::Color::Red] = "Red";
	this->m_colorMap[sf::Color::Green] = "Green";
	this->m_colorMap[sf::Color::Blue] = "Blue";
	this->m_colorMap[sf::Color::Yellow] = "Yellow";
	this->m_colorMap[sf::Color::Magenta] = "Magenta";
	this->m_colorMap[sf::Color::Cyan] = "Cyan";

	return true;
}

bool Login::initWindow()
{
	this->m_videoMode.width = 1920;
	this->m_videoMode.height = 1080;
	this->m_window = new sf::RenderWindow(this->m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	this->m_window->setVerticalSyncEnabled(true);

	// Create UI for login screen
	// Font
	if (!this->m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
		return false;
	}

	// m_title Text
	this->m_title = sf::Text("Welcome to Nottingham!", this->m_font, 48);
	this->m_title.setFillColor(sf::Color::White);
	this->m_title.setPosition((1920 - m_title.getLocalBounds().width) / 2, 250);

	// Username Label
	this->m_usernameLabel = sf::Text("Username:", m_font, 24);
	this->m_usernameLabel.setFillColor(sf::Color::White);
	this->m_usernameLabel.setPosition(700, 410);

	// IP Label
	this->m_IPLabel = sf::Text("IP Address:", m_font, 24);
	this->m_IPLabel.setFillColor(sf::Color::White);
	this->m_IPLabel.setPosition(700, 510);

	// Username Input Box
	this->m_usernameBox = sf::RectangleShape(sf::Vector2f(400, 50));
	this->m_usernameBox.setFillColor(sf::Color(200, 200, 200));
	this->m_usernameBox.setPosition(850, 400);

	// IP Input Box
	this->m_IPBox = sf::RectangleShape(sf::Vector2f(400, 50));
	this->m_IPBox.setFillColor(sf::Color(200, 200, 200));
	this->m_IPBox.setPosition(850, 500);

	// Login Button
	this->m_loginButton = sf::RectangleShape(sf::Vector2f(200, 50));
	this->m_loginButton.setFillColor(sf::Color::Blue);
	this->m_loginButton.setPosition((1920 - 200) / 2, 750);

	// Login Button Text
	this->m_loginButtonText = sf::Text("Login", m_font, 24);
	this->m_loginButtonText.setFillColor(sf::Color::White);
	this->m_loginButtonText.setPosition(
		this->m_loginButton.getPosition().x + (this->m_loginButton.getSize().x - this->m_loginButtonText.getLocalBounds().width) / 2,
		this->m_loginButton.getPosition().y + (this->m_loginButton.getSize().y - this->m_loginButtonText.getLocalBounds().height) / 2 - 5
	);

	// Create color boxes
	float colorBoxSize = 50; // Size of each color box
	float spacing = 10; // Spacing between boxes

	for (size_t i = 0; i < m_availableColor.size(); i++) {
		sf::RectangleShape colorBox(sf::Vector2f(colorBoxSize, colorBoxSize));
		colorBox.setFillColor(m_availableColor[i]);
		colorBox.setPosition(850 + i * (colorBoxSize + spacing), 600);
		m_colorOptions.push_back(colorBox);
	}

	// Initialize selected color highlight
	m_selectedColorOutline = sf::RectangleShape(sf::Vector2f(colorBoxSize, colorBoxSize));
	m_selectedColorOutline.setFillColor(sf::Color::Transparent);
	m_selectedColorOutline.setOutlineColor(sf::Color::White);
	m_selectedColorOutline.setOutlineThickness(2);
	m_selectedColorOutline.setPosition(m_colorOptions[0].getPosition());
	m_selectedColor = m_availableColor[0]; // Default selected color

	return true;
}

Login::Login()
{
	this->initVariables();
	this->initWindow();
}

Login::~Login()
{
	delete this->m_window;
}

const bool Login::running() const
{
	return this->m_window->isOpen();
}

bool Login::pollEvents()
{
	while (this->m_window->pollEvent(this->m_ev)) {
		switch (this->m_ev.type) {
		// Case close window
		case sf::Event::Closed:
			this->m_window->close();
			break;

		// Case left click
		case sf::Event::MouseButtonPressed:
			this->m_mousePressedPos = INVALID;
			if (this->m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXY = sf::Mouse::getPosition(*this->m_window);
				if (this->m_loginButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					this->m_mousePressedPos = LOGIN_BUTTON;
				}
				else if (this->m_usernameBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					this->m_mousePressedPos = USERNAME_BOX;
				}
				else if (this->m_IPBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					this->m_mousePressedPos = IP_BOX;
				}
				else {
					for (size_t i = 0; i < m_colorOptions.size(); i++) {
						if (m_colorOptions[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
							// Store selected color and outline
							m_selectedColor = m_colorOptions[i].getFillColor();
							m_selectedColorOutline.setPosition(m_colorOptions[i].getPosition());
						}
					}
				}
			}
			break;
		
		// Case enter login info
		case sf::Event::TextEntered:
			if (this->m_mousePressedPos == USERNAME_BOX) {
				if (this->m_ev.text.unicode == '\b' && !m_usernameText.empty()) { // Backspace
					m_usernameText.pop_back();
				}
				else if (this->m_ev.text.unicode >= 32 && this->m_ev.text.unicode <= 126) { // Printable characters
					m_usernameText += static_cast<char>(this->m_ev.text.unicode);
				}
			}
			else if (this->m_mousePressedPos == IP_BOX) {
				if (this->m_ev.text.unicode == '\b' && !m_IPText.empty()) { // Backspace
					m_IPText.pop_back();
				}
				else if (this->m_ev.text.unicode >= 32 && this->m_ev.text.unicode <= 126) { // Printable characters
					m_IPText += static_cast<char>(this->m_ev.text.unicode);
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
	this->pollEvents();

	// Username text display
	this->m_usernameDisplay = sf::Text(this->m_usernameText, m_font, 24);
	this->m_usernameDisplay.setFillColor(sf::Color::Black);
	this->m_usernameDisplay.setPosition(m_usernameBox.getPosition().x + 10, m_usernameBox.getPosition().y + 10);

	// IP Address display
	this->m_IPDisplay = sf::Text(m_IPText, m_font, 24);
	this->m_IPDisplay.setFillColor(sf::Color::Black);
	this->m_IPDisplay.setPosition(m_IPBox.getPosition().x + 10, m_IPBox.getPosition().y + 10);

	if (this->m_mousePressedPos == LOGIN_BUTTON) {
		m_mousePressedPos = INVALID;
		// Send infomation to server
		std::cout << "Login infomation sending to server...\n";
		std::cout << "Username: " << this->m_usernameText << "\nIP Address: " << this->m_IPText << "\n";
		std::cout << "Selected color: " << m_colorMap[m_selectedColor] << "\n";
	}
	return true;
}

bool Login::render()
{
	// Clear screen to black screen
	this->m_window->clear();

	// Draw login screen
	this->m_window->draw(m_title);
	this->m_window->draw(m_usernameBox);
	this->m_window->draw(m_usernameLabel);

	this->m_window->draw(m_IPBox);
	this->m_window->draw(m_IPLabel);

	this->m_window->draw(m_usernameDisplay);
	this->m_window->draw(m_IPDisplay);

	for (const auto& colorBox : m_colorOptions) {
		this->m_window->draw(colorBox);
	}
	this->m_window->draw(m_selectedColorOutline);

	this->m_window->draw(m_loginButton);
	this->m_window->draw(m_loginButtonText);

	

	// Display
	this->m_window->display();
	
	return true;
}
