#ifndef LOGIN__
#define LOGIN__

#include <iostream>
#include <SFML/Graphics.hpp>

class Login
{
public:
	enum MousePressedPosition {
		INVALID,
		USERNAME_BOX,
		PASSWORD_BOX,
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
	sf::Text m_passwordLabel;
	sf::RectangleShape m_usernameBox;
	sf::RectangleShape m_passwordBox;
	sf::Text m_usernameDisplay;
	sf::Text m_passwordDisplay;
	sf::RectangleShape m_loginButton;
	sf::Text m_loginButtonText;
	std::string m_usernameText;
	std::string m_passwordText;
	

	MousePressedPosition m_mousePressedPos;

	// Init
	bool initVariables();
	bool initWindow();
};

#endif // !LOGIN__

