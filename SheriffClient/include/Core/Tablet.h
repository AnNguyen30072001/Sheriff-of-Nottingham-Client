#ifndef _TABLET__
#define _TABLET__

#include <SFML/Graphics.hpp>
#include "../include/Entities/Card.h"
#include "../include/Entities/Player.h"

class Tablet {
public:
	enum class Type {
		GIVE_BAG,
		INFO
	};

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
	void showTablet(Tablet::Type type, int maxMoney, Player* player = nullptr);
	void hideTablet();
	void resetOptions();

private:
	sf::RenderWindow* m_parentWindow;
	sf::Event m_ev;
	sf::Font m_font;
	Type m_tabletType;

	sf::RectangleShape	m_tabletGiveBag;				// Tablet background
	sf::RectangleShape	m_tabletInfo;				
	sf::Texture			m_tabletTextureGiveBag;
	sf::Texture			m_tabletTextureInfo;
	sf::RectangleShape	m_backgroundMask;				// Window background black mask

	sf::CircleShape		m_playerAvatar;					// Player avatar
	sf::CircleShape		m_playerAvatarFrame;			// Player avatar frame
	sf::Text			m_playerName;

	sf::RectangleShape	m_moneyIcon;					// Money icon
	sf::Texture			m_moneyIconTexture;
	sf::Text			m_moneyText;

	sf::RectangleShape	m_scoreIcon;					// Score icon
	sf::Texture			m_scoreIconTexture;
	sf::Text			m_scoreText;

	sf::RectangleShape	m_goldMedals[4];				// Gold medals for legal goods
	sf::Texture			m_goldMedalTexture;
	sf::RectangleShape	m_silverMedals[4];				// Silver medals for legal goods
	sf::Texture			m_silverMedalTexture;
	sf::RectangleShape	m_blackMarketMedals[4];			// Black market medals for contrabands
	sf::Texture			m_blackMarketMedalTexture;

	sf::Text			m_goodsAmountText[8];

	sf::RectangleShape	m_okButton;						// OK button
	sf::Text			m_okButtonText;

	sf::RectangleShape	m_goodsButtons[8]; // Buttons for Apple, Bread, Cheese, Chicken, Crossbow, Mead, Pepper, Silk
	sf::Texture			m_goodsButtonTextures[4];
	sf::Texture			m_goodsButtonTexturesHighLight[8];

	sf::Text			m_bribeText;
	sf::RectangleShape	m_bribeBox;
	sf::Text			m_bribeBoxText;
	std::string			m_bribeInput;

	sf::RectangleShape	m_BoxOutline;
	sf::Text			m_maxMoneyText;

	Card::CardType m_selectedGoods; // The goods that merchant reports
	int m_playerMoney;
	int m_playerScore;
	unsigned int m_bribeAmount; // Selected bribe amount

	bool m_isTabletVisible; // To toggle tablet visibility
	bool m_confirm;

	void setupGiveBagUI();
	void setupInfoUI(Player* player = nullptr);

	bool confirmSelection();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool handleMouseDrag(sf::Vector2f mousePosXY);
	void handleTextEnter();
};


#endif // !_TABLET__


