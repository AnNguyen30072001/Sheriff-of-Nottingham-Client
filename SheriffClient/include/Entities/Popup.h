#ifndef _POP_UP__
#define _POP_UP__

#include <SFML/Graphics.hpp>

class Popup
{
public:
	Popup(sf::RenderWindow* parentWindow, float width, float height);
	~Popup();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool pollEvents();
	bool render();

	sf::Text getPopupText() const;
	void setMessage(const std::string& message);
	void show();
	void hide();
	bool isVisible();

private:
	sf::RenderWindow* m_parentWindow;
	sf::VideoMode m_videoMode;
	sf::Event m_ev;

	sf::RectangleShape m_background; // Popup background
	sf::Text m_popupText;            // Text for the popup content
	sf::RectangleShape m_okButton;   // "OK" button
	sf::Text m_okText;               // Text for the "OK" button
	sf::Font m_font;                 // Font for the popup

	bool m_isVisible;

	void centerText(sf::Text& text, sf::RectangleShape& button);
	void centerPopupText();
	void centerButton();
};


#endif // !_POP_UP__



