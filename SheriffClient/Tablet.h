#ifndef _TABLET__
#define _TABLET__

#include <SFML/Graphics.hpp>
#include "Card.h"

class Tablet {
public:
	Tablet(sf::RenderWindow* parentWindow);
	~Tablet();

	// Get render targets
	sf::RectangleShape getTablet() const;
	sf::RectangleShape getOkButton() const;
	sf::RectangleShape getGoodsButton(uint8_t index) const;
	sf::Text getBribeText() const;

	// Get infomation
	unsigned int getBribeAmount() const;
	Card::CardType getPresentedGoods() const;
	bool isPresentConfirmed();

	bool pollEvents();
	void update();
	void render();

	bool isTabletVisible() const;
	void showTablet(int maxMoney);
	void hideTablet();
	void resetOptions();

private:
	sf::RenderWindow* m_parentWindow;
	sf::Event m_ev;
	sf::Font m_font;

	sf::RectangleShape m_tablet; // Tablet background
	sf::Texture m_tabletTexture;
	sf::RectangleShape m_okButton; // OK button
	sf::Text m_okButtonText;
	sf::RectangleShape m_goodsButtons[4]; // Buttons for Apple, Bread, Cheese, Chicken 
	sf::Texture m_goodsButtonTextures[4];
	sf::Texture m_goodsButtonTexturesHighLight[4];
	sf::Text m_bribeText;
	sf::RectangleShape m_bribeBox;
	sf::Text m_bribeBoxText;
	std::string m_bribeInput;
	sf::RectangleShape m_BoxOutline;
	sf::Text m_maxMoneyText;

	Card::CardType m_selectedGoods; // The goods that merchant reports
	int m_playerMoney;
	unsigned int m_bribeAmount; // Selected bribe amount

	bool m_isTabletVisible; // To toggle tablet visibility
	bool m_confirm;

	bool confirmSelection();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool handleMouseDrag(sf::Vector2f mousePosXY);
	void handleTextEnter();
};


#endif // !_TABLET__


