#include "login.h"

bool Login::initVariables()
{
	this->m_window = nullptr;
	this->m_mousePressedPos = INVALID;
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

	// Password Label
	this->m_passwordLabel = sf::Text("Password:", m_font, 24);
	this->m_passwordLabel.setFillColor(sf::Color::White);
	this->m_passwordLabel.setPosition(700, 510);

	// Username Input Box
	this->m_usernameBox = sf::RectangleShape(sf::Vector2f(400, 50));
	this->m_usernameBox.setFillColor(sf::Color(200, 200, 200));
	this->m_usernameBox.setPosition(850, 400);

	// Password Input Box
	this->m_passwordBox = sf::RectangleShape(sf::Vector2f(400, 50));
	this->m_passwordBox.setFillColor(sf::Color(200, 200, 200));
	this->m_passwordBox.setPosition(850, 500);

	// Login Button
	this->m_loginButton = sf::RectangleShape(sf::Vector2f(200, 50));
	this->m_loginButton.setFillColor(sf::Color::Blue);
	this->m_loginButton.setPosition((1920 - 200) / 2, 650);

	// Login Button Text
	this->m_loginButtonText = sf::Text("Login", m_font, 24);
	this->m_loginButtonText.setFillColor(sf::Color::White);
	this->m_loginButtonText.setPosition(
		this->m_loginButton.getPosition().x + (this->m_loginButton.getSize().x - this->m_loginButtonText.getLocalBounds().width) / 2,
		this->m_loginButton.getPosition().y + (this->m_loginButton.getSize().y - this->m_loginButtonText.getLocalBounds().height) / 2 - 5
	);

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
			if (this->m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXY = sf::Mouse::getPosition(*this->m_window);
				if (this->m_loginButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					this->m_mousePressedPos = LOGIN_BUTTON;
				}
				else if (this->m_usernameBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					this->m_mousePressedPos = USERNAME_BOX;
				}
				else if (this->m_passwordBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
					this->m_mousePressedPos = PASSWORD_BOX;
				}
				else {
					this->m_mousePressedPos = INVALID;
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
			else if (this->m_mousePressedPos == PASSWORD_BOX) {
				if (this->m_ev.text.unicode == '\b' && !m_passwordText.empty()) { // Backspace
					m_passwordText.pop_back();
				}
				else if (this->m_ev.text.unicode >= 32 && this->m_ev.text.unicode <= 126) { // Printable characters
					m_passwordText += static_cast<char>(this->m_ev.text.unicode);
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

	// Password display
	std::string maskedPassword(m_passwordText.size(), '*');
	this->m_passwordDisplay = sf::Text(maskedPassword, m_font, 24);
	this->m_passwordDisplay.setFillColor(sf::Color::Black);
	this->m_passwordDisplay.setPosition(m_passwordBox.getPosition().x + 10, m_passwordBox.getPosition().y + 15);

	if (this->m_mousePressedPos == LOGIN_BUTTON) {
		m_mousePressedPos = INVALID;
		// Send infomation to server
		std::cout << "Login infomation sending to server...\n";
		std::cout << "Username: " << this->m_usernameText << "\nPass: " << this->m_passwordText << "\n";
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
	this->m_window->draw(m_passwordBox);
	this->m_window->draw(m_passwordLabel);
	this->m_window->draw(m_usernameDisplay);
	this->m_window->draw(m_passwordDisplay);
	this->m_window->draw(m_loginButton);
	this->m_window->draw(m_loginButtonText);

	// Display
	this->m_window->display();
	
	return true;
}
