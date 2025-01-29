#include "Tablet.h"
#include <iostream>

Tablet::Tablet(sf::RenderWindow* parentWindow)
{
	m_bribeAmount = 0;
	m_playerMoney = 0;
	m_selectedGoods = Card::INVALID;
	m_parentWindow = parentWindow;
	m_isTabletVisible = false;
	m_confirm = false;

	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading tablet font!" << std::endl;
	}

	if (!m_tabletTexture.loadFromFile("Images/ReportBackground.png")) {
		std::cerr << "Error loading tablet background!" << std::endl;
	}
}

Tablet::~Tablet()
{

}

sf::RectangleShape Tablet::getTablet() const
{
	return m_tablet;
}

sf::RectangleShape Tablet::getOkButton() const
{
	return m_okButton;
}

sf::RectangleShape Tablet::getGoodsButton(uint8_t index) const
{
	return m_goodsButtons[index];
}

bool Tablet::handleMouseClick(sf::Vector2f mousePosXY)
{
	// If press outside of tablet, close tablet and cancel all previous choices
	if (!m_tablet.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
		hideTablet();
		resetOptions();

		return true;
	}

	// Handle goods selection
	for (int i = 0; i < 4; ++i) {
		if (m_goodsButtons[i].getGlobalBounds().contains(mousePosXY)) {
			m_selectedGoods = static_cast<Card::CardType>(i + 1);
			// Highlight selected button
			for (int j = 0; j < 4; ++j) {
				m_goodsButtons[j].setTexture(&m_goodsButtonTextures[j]);
			}
			m_goodsButtons[i].setTexture(&m_goodsButtonTexturesHighLight[i]); // Highlight

			return true;
		}
	}

	// Enter bribe amount
	if (m_bribeBox.getGlobalBounds().contains(mousePosXY)) {
		m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 255));
	}
	else {
		// Click outside the box
		m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 0));
	}

	// Check OK button
	if (m_okButton.getGlobalBounds().contains(mousePosXY)) {
		m_confirm = confirmSelection();

		return true;
	}

	return false;
}

bool Tablet::handleMouseDrag(sf::Vector2f mousePosXY)
{

	return false;
}

void Tablet::handleTextEnter()
{
	if (!m_isTabletVisible) return;  // Only accept input if the tablet is open

	if (m_ev.type == sf::Event::TextEntered) {
		if (m_ev.text.unicode == 8) {  // Backspace key
			if (!m_bribeInput.empty()) {
				m_bribeInput.pop_back();
			}
		}
		else if (m_ev.text.unicode >= '0' && m_ev.text.unicode <= '9') { // Only allow numbers
			m_bribeInput += static_cast<char>(m_ev.text.unicode);
		}

		// Convert string to number and enforce max value
		m_bribeAmount = (m_bribeInput.empty()) ? 0 : std::stoul(m_bribeInput);
		if (m_bribeAmount > m_playerMoney) {
			m_bribeAmount = m_playerMoney;
			m_bribeInput = std::to_string(m_playerMoney);
		}

		// Update displayed text
		m_bribeBoxText.setString(m_bribeInput.empty() ? "0" : m_bribeInput);

		// Get text bounds
		sf::FloatRect textBounds = m_bribeBoxText.getLocalBounds();
		// Center the text within m_bribeBox
		float centerX = m_bribeBox.getPosition().x + (m_bribeBox.getSize().x - textBounds.width) / 2;
		float centerY = m_bribeBox.getPosition().y + (m_bribeBox.getSize().y - textBounds.height) / 2;
		// Adjust for text baseline offset
		centerX -= textBounds.left;
		centerY -= textBounds.top;
		m_bribeBoxText.setPosition(centerX, centerY);
	}
}

sf::Text Tablet::getBribeText() const
{
	return m_bribeText;
}

unsigned int Tablet::getBribeAmount() const
{
	return m_bribeAmount;
}

Card::CardType Tablet::getPresentedGoods() const
{
	return m_selectedGoods;
}

bool Tablet::isPresentConfirmed()
{
	return m_confirm;
}

bool Tablet::pollEvents()
{
	if (!m_isTabletVisible) return false;

	while (m_parentWindow->pollEvent(m_ev)) {
		switch (m_ev.type) {
		case sf::Event::MouseButtonPressed:
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_parentWindow);
				sf::Vector2f mousePosXY = m_parentWindow->mapPixelToCoords(mousePosXYLocal);
				handleMouseClick(mousePosXY);
			}
			break;

		case sf::Event::TextEntered:
			// Can only type if box is selected
			if (m_BoxOutline.getFillColor() == sf::Color(255, 255, 255, 255)) {
				handleTextEnter();
			}

			break;
		//case sf::Event::MouseMoved:
		//	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//		sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_parentWindow);
		//		sf::Vector2f mousePosXY = m_parentWindow->mapPixelToCoords(mousePosXYLocal);
		//		handleMouseDrag(mousePosXY);
		//	}
		//	break;

		default:
			break;
		}
	}

	return true;
}

void Tablet::update()
{
	pollEvents();
}

void Tablet::render()
{
	// This is sub-content, so no clear() or display()
	m_parentWindow->draw(m_tablet);
	for (auto& button : m_goodsButtons) {
		m_parentWindow->draw(button);
	}
	m_parentWindow->draw(m_bribeText);
	m_parentWindow->draw(m_bribeBox);
	m_parentWindow->draw(m_BoxOutline);
	m_parentWindow->draw(m_bribeBoxText);
	m_parentWindow->draw(m_maxMoneyText);
	m_parentWindow->draw(m_okButton);
	m_parentWindow->draw(m_okButtonText);
}

bool Tablet::isTabletVisible() const
{
	return m_isTabletVisible;
}

void Tablet::showTablet(int maxMoney)
{
	m_playerMoney = maxMoney;
	m_isTabletVisible = true;

	// Tablet background
	m_tablet.setSize({ 750, 400 });
	m_tablet.setFillColor(sf::Color::White);
	m_tablet.setTexture(&m_tabletTexture);
	m_tablet.setPosition(m_parentWindow->getSize().x / 2 - m_tablet.getSize().x / 2, 150);

	// Goods buttons
	std::string texturePaths[4] = { "AppleReport.png", "BreadReport.png", "CheeseReport.png", "ChickenReport.png" };
	std::string texturePathsHighlight[4] = { "AppleReportHighlight.png", "BreadReportHighlight.png", "CheeseReportHighlight.png", "ChickenReportHighlight.png" };
	for (int i = 0; i < 4; ++i) {
		m_goodsButtonTextures[i].loadFromFile("Images/" + texturePaths[i]);
		m_goodsButtonTexturesHighLight[i].loadFromFile("Images/" + texturePathsHighlight[i]);
		m_goodsButtons[i].setSize(sf::Vector2f(100, 100));
		m_goodsButtons[i].setFillColor(sf::Color::White);
		m_goodsButtons[i].setTexture(&m_goodsButtonTextures[i]);
		m_goodsButtons[i].setPosition(670.f + i * 155.f, 190.f); // Position buttons in a row
	}

	// Bribe text
	m_bribeText.setFont(m_font);
	m_bribeText.setFillColor(sf::Color::Black);
	m_bribeText.setString("Bribe:");
	m_bribeText.setCharacterSize(32);
	m_bribeText.setPosition(670.f, 317.f);

	// Bribe box
	m_bribeBox.setSize(sf::Vector2f(120.f, 50.f));
	m_bribeBox.setFillColor(sf::Color(200, 200, 200));
	m_bribeBox.setPosition(770.f, 312.f);
	m_BoxOutline.setSize(m_bribeBox.getSize());
	m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 0));
	m_BoxOutline.setPosition(m_bribeBox.getPosition());

	// Bribe text inside box
	m_bribeBoxText.setFont(m_font);
	m_bribeBoxText.setCharacterSize(28);
	m_bribeBoxText.setFillColor(sf::Color::Black);
	m_bribeBoxText.setString("0");
	// Get text bounds
	sf::FloatRect textBounds = m_bribeBoxText.getLocalBounds();
	// Center the text within m_bribeBox
	float centerX = m_bribeBox.getPosition().x + (m_bribeBox.getSize().x - textBounds.width) / 2;
	float centerY = m_bribeBox.getPosition().y + (m_bribeBox.getSize().y - textBounds.height) / 2;
	// Adjust for text baseline offset
	centerX -= textBounds.left;
	centerY -= textBounds.top;
	m_bribeBoxText.setPosition(centerX, centerY);

	// Current money text
	m_maxMoneyText.setFont(m_font);
	m_maxMoneyText.setFillColor(sf::Color::Black);
	m_maxMoneyText.setString("Current money: " + std::to_string(m_playerMoney) + "g");
	m_maxMoneyText.setCharacterSize(28);
	m_maxMoneyText.setPosition(670.f, 377.f);

	// OK button
	m_okButton.setSize({ 160, 70 });
	m_okButton.setFillColor(sf::Color(100, 149, 237));
	m_okButton.setPosition(880.f, 435.f);

	// OK text
	m_okButtonText.setFont(m_font);
	m_okButtonText.setString("Confirm");
	m_okButtonText.setCharacterSize(32);
	m_okButtonText.setPosition(900.f, 450.f);

}

void Tablet::hideTablet()
{
	m_isTabletVisible = false;
}

void Tablet::resetOptions()
{
	m_confirm = false;
	m_bribeAmount = 0;
	m_playerMoney = 0;
	m_bribeInput = "";
	m_selectedGoods = Card::INVALID;
}

bool Tablet::confirmSelection()
{
	if (m_selectedGoods == Card::INVALID) {
		// Return early
		return false;
	}

	// Process the selection
	std::cout << "Reported goods: " << Card::m_cardNameToString.at(m_selectedGoods)
		<< ", Bribe: " << m_bribeAmount << " Gold" << std::endl;

	hideTablet(); // Hide tablet UI
	return true;
}