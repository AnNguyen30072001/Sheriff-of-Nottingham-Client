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
	unsigned int getBribedGoodsAmount() const;
	Card::CardType getPresentedGoods() const;
	Card::CardType getBribedGoodsType() const;
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
	sf::Texture			m_tabletTextureInfoUser;
	sf::RectangleShape	m_backgroundMask;				// Window background black mask

	sf::CircleShape		m_playerAvatar;					// Player avatar
	sf::CircleShape		m_playerAvatarFrame;			// Player avatar frame
	sf::Text			m_playerName;
	sf::RectangleShape	m_playerNameBox;

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
	sf::RectangleShape	m_blackMarketMedals[6];			// Black market medals for contrabands
	sf::Texture			m_blackMarketMedalTexture[6];

	sf::Text			m_goodsAmountText[8];

	sf::RectangleShape	m_okButton;						// OK button
	sf::Text			m_okButtonText;

	sf::RectangleShape	m_goodsButtons[8]; // Buttons for Apple, Bread, Cheese, Chicken, Crossbow, Mead, Pepper, Silk
	sf::Texture			m_goodsButtonTextures[8];
	sf::Texture			m_goodsButtonTexturesHighLight[8];
	sf::RectangleShape	m_goodsBribeButtons[8];	// Buttons to select bribed goods. These buttons use same texture as m_goodsButtons

	sf::Text			m_giveBagInfoText1;
	sf::Text			m_giveBagInfoText2;
	sf::Text			m_bribeMoneyText;
	sf::RectangleShape	m_bribeMoneyBox;
	sf::Text			m_bribeMoneyBoxText;
	std::string			m_bribeMoneyInput;
	sf::Text			m_bribeGoodsText;
	sf::RectangleShape	m_bribeGoodsBox;
	sf::Text			m_bribeGoodsBoxText;
	std::string			m_bribeGoodsAmountInput;
	Card::CardType		m_bribeGoodsTypeInput;

	sf::RectangleShape	m_BoxOutline;
	sf::Text			m_maxMoneyText;

	Card::CardType m_selectedGoods; // The goods that merchant reports
	int m_playerMoney;
	int m_playerScore;
	unsigned int m_bribeMoneyAmount; // Selected bribe money amount
	unsigned int m_bribeGoodsAmount; // Selected bribe goods amount

	bool m_isTabletVisible; // To toggle tablet visibility
	bool m_confirm;

	void setupGiveBagUI();
	void setupInfoUI(Player* player = nullptr);

	bool confirmSelection();

	bool handleMouseClick(sf::Vector2f mousePosXY);
	bool handleMouseDrag(sf::Vector2f mousePosXY);
	void handleTextEnter();

	void centerText(sf::Text & text, sf::RectangleShape & button);
};


#endif // !_TABLET__


