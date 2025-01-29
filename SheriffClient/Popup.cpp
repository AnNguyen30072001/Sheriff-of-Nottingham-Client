#include <iostream>
#include "Popup.h"

Popup::Popup(sf::RenderWindow* parentWindow, float width, float height)
{
	m_parentWindow = parentWindow;

	// Load font
	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	// Initialize the popup
	m_background.setSize(sf::Vector2f(width, height));
	m_background.setFillColor(sf::Color(50, 50, 50, 200)); // Semi-transparent dark gray
	m_background.setOutlineThickness(3.f);
	m_background.setOutlineColor(sf::Color::White);
	m_background.setPosition((parentWindow->getSize().x - width) / 2.f, (parentWindow->getSize().y - height) / 2.f);

	// Popup text
	m_popupText.setFont(m_font);
	m_popupText.setCharacterSize(24);
	m_popupText.setFillColor(sf::Color::White);
	m_popupText.setOutlineColor(sf::Color::Black);
	m_popupText.setOutlineThickness(1.f);
	m_popupText.setPosition(m_background.getPosition().x + 20.f, m_background.getPosition().y + 20.f);

	// OK button
	m_okButton.setSize(sf::Vector2f(100.f, 40.f));
	m_okButton.setFillColor(sf::Color(100, 149, 237));
	m_okButton.setPosition(m_background.getPosition().x + 150.f, m_background.getPosition().y + 130.f);

	// OK button text
	m_okText.setFont(m_font);
	m_okText.setCharacterSize(20);
	m_okText.setFillColor(sf::Color::White);
	m_okText.setOutlineColor(sf::Color::Black);
	m_okText.setOutlineThickness(1.f);
	m_okText.setString("OK");

	m_isVisible = false; // Initially hidden
}

Popup::~Popup()
{

}

bool Popup::handleMouseClick(sf::Vector2f mousePosXY)
{
	if (m_okButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY))) {
		hide();
	}

	return true;
}

bool Popup::pollEvents()
{
	if (!m_isVisible) return false;

	while (m_parentWindow->pollEvent(m_ev)) {
		switch (m_ev.type) {
		case sf::Event::MouseButtonPressed:
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_parentWindow);
				sf::Vector2f mousePosXY = m_parentWindow->mapPixelToCoords(mousePosXYLocal);
				handleMouseClick(mousePosXY);
			}
			break;
		default:
			break;
		}
	}

	return true;
}

bool Popup::render()
{
	if (!m_isVisible) return false;

	m_parentWindow->draw(m_background);
	m_parentWindow->draw(m_popupText);
	m_parentWindow->draw(m_okButton);
	m_parentWindow->draw(m_okText);

	return true;
}

sf::Text Popup::getPopupText() const
{
	return m_popupText;
}

void Popup::setMessage(const std::string& message)
{
	m_popupText.setString(message);
	centerPopupText();
}

void Popup::show()
{
	m_isVisible = true;
	centerButton();
}

void Popup::hide()
{
	m_isVisible = false;
}

bool Popup::isVisible()
{
	return m_isVisible;
}

void Popup::centerText(sf::Text & text, sf::RectangleShape & button)
{
	float x = button.getPosition().x + (button.getSize().x - text.getLocalBounds().width) / 2.f;
	float y = button.getPosition().y + (button.getSize().y - text.getLocalBounds().height) / 2.f - 5.f;
	text.setPosition(x, y);
}

void Popup::centerPopupText()
{
	float x = m_background.getPosition().x + (m_background.getSize().x - m_popupText.getLocalBounds().width) / 2.f;
	float y = m_background.getPosition().y + (m_background.getSize().y - m_popupText.getLocalBounds().height) / 2.f - 30.f;
	m_popupText.setPosition(x, y);
}

void Popup::centerButton()
{
	float x = m_background.getPosition().x + (m_background.getSize().x - m_okButton.getSize().x) / 2.f;
	float y = m_background.getPosition().y + m_background.getSize().y - 70.f;
	m_okButton.setPosition(x, y);
	centerText(m_okText, m_okButton);
}
