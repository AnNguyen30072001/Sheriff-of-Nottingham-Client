#ifndef LOGIN__
#define LOGIN__

#include <iostream>
#include <unordered_map>
#include "ColorMap.h"
#include <SFML/Graphics.hpp>

class Login
{
public:
	enum MousePressedPosition {
		INVALID,
		USERNAME_BOX,
		IP_BOX,
		LOGIN_BUTTON
	};
	Login();
	virtual ~Login();

	// Accessors
	const bool running() const;

	// Functions 
	bool pollEvents();
	bool update();
	bool render();

private:
	sf::RenderWindow* m_window;
	sf::Event m_ev;
	sf::VideoMode m_videoMode;

	sf::Font m_font;
	sf::Text m_title;
	sf::Text m_usernameLabel;
	sf::Text m_IPLabel;
	sf::RectangleShape m_usernameBox;
	sf::RectangleShape m_IPBox;
	sf::RectangleShape m_BoxOutline;
	sf::Text m_usernameDisplay;
	sf::Text m_IPDisplay;
	sf::RectangleShape m_loginButton;
	sf::Text m_loginButtonText;
	std::string m_usernameText;
	std::string m_IPText;
	std::vector<sf::RectangleShape> m_colorOptions; // Predefined color options
	sf::RectangleShape m_selectedColorOutline;     // Highlight for selected color
	sf::Color m_selectedColor;
	std::vector<sf::Color> m_availableColor;
	std::unordered_map<sf::Color, std::string, ColorHash, ColorEqual> m_colorMap;

	MousePressedPosition m_mousePressedPos;

	// Init
	bool initVariables();
	bool initWindow();
};

#endif // !LOGIN__

