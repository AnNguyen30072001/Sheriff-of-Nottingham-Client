#include "../include/Core/Tablet.h"
#include <iostream>

#define BRIBE_GOODS_AMOUNT_MAX	99U

Tablet::Tablet(sf::RenderWindow* parentWindow)
{
	m_bribeMoneyAmount = 0;
	m_bribeGoodsAmount = 0;
	m_playerMoney = 0;
	m_playerScore = 0;
	m_selectedGoods = Card::INVALID;
	m_bribeGoodsTypeInput = Card::INVALID;
	m_parentWindow = parentWindow;
	m_isTabletVisible = false;
	m_confirm = false;

	if (!m_font.loadFromFile("assets/arial-font/arial.ttf")) {
		std::cerr << "Error loading tablet font!" << std::endl;
	}

	if (!m_goldMedalTexture.loadFromFile("assets/Images/Tokens/GoldToken.png") || 
		!m_silverMedalTexture.loadFromFile("assets/Images/Tokens/SilverToken.png") || 
		!m_moneyIconTexture.loadFromFile("assets/Images/Tokens/MoneyIcon.png") || 
		!m_scoreIconTexture.loadFromFile("assets/Images/Tokens/ScoreIcon.png")) {
		std::cerr << "Error loading tablet texture!" << std::endl;
	}

	if (!m_tabletTextureGiveBag.loadFromFile("assets/Images/ReportBackground.png")) {
		std::cerr << "Error loading tablet background!" << std::endl;
	}

	if (!m_tabletTextureInfo.loadFromFile("assets/Images/ReportBackground_2.png")) {
		std::cerr << "Error loading tablet background!" << std::endl;
	}

	std::string blackMedalTexturePaths[6] = {
		"PepperToken_v1.png", "PepperToken_v2.png",
		"MeadToken_v1.png", "MeadToken_v2.png",
		"SilkToken_v1.png", "SilkToken_v2.png"
	};
	for (int i = 0; i < 6; i++) {
		if (!m_blackMarketMedalTexture[i].loadFromFile("assets/Images/Tokens/" + blackMedalTexturePaths[i])) {
			std::cerr << "Error loading tablet texture!" << std::endl;
		}
	}

	m_backgroundMask.setSize(sf::Vector2f(1920.f, 1080.f));
	m_backgroundMask.setFillColor(sf::Color(0, 0, 0, 180));
}

Tablet::~Tablet()
{

}

sf::RectangleShape Tablet::getTablet() const
{
	switch (m_tabletType)
	{
	case Tablet::Type::GIVE_BAG:
		return m_tabletGiveBag;

		break;
	case Tablet::Type::INFO:
		return m_tabletInfo;

		break;
	default:
		break;
	}
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
	if (!m_tabletGiveBag.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_tabletType == Tablet::Type::GIVE_BAG) {
		hideTablet();
		resetOptions();

		return true;
	}

	else if (!m_tabletInfo.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_tabletType == Tablet::Type::INFO) {
		hideTablet();
		resetOptions();

		return true;
	}

	// Handle goods selection
	for (int i = 0; i < 8; ++i) {
		if ( (i < 4) && (m_goodsButtons[i].getGlobalBounds().contains(mousePosXY)) && (m_tabletType == Tablet::Type::GIVE_BAG) ) {
			// Store information
			m_selectedGoods = static_cast<Card::CardType>(i + 1);

			for (int j = 0; j < 4; ++j) {
				m_goodsButtons[j].setTexture(&m_goodsButtonTextures[j]);
			}
			// Highlight selected button
			m_goodsButtons[i].setTexture(&m_goodsButtonTexturesHighLight[i]);
		}

		else if ( (m_goodsBribeButtons[i].getGlobalBounds().contains(mousePosXY)) && (m_tabletType == Tablet::Type::GIVE_BAG) ) {
			// Store information
			m_bribeGoodsTypeInput = static_cast<Card::CardType>(i + 1);

			for (int j = 0; j < 8; ++j) {
				m_goodsBribeButtons[j].setTexture(&m_goodsButtonTextures[j]);
			}
			// Highlight selected button
			m_goodsBribeButtons[i].setTexture(&m_goodsButtonTexturesHighLight[i]);
		}
	}

	// Click inside bribe box
	if (m_bribeMoneyBox.getGlobalBounds().contains(mousePosXY) && m_tabletType == Tablet::Type::GIVE_BAG) {
		m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 255));
		m_BoxOutline.setPosition(m_bribeMoneyBox.getPosition());
	}
	else if (m_bribeGoodsBox.getGlobalBounds().contains(mousePosXY) && m_tabletType == Tablet::Type::GIVE_BAG) {
		m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 255));
		m_BoxOutline.setPosition(m_bribeGoodsBox.getPosition());
	}
	else {
		// Click outside the box
		m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 0));
	}

	// Check OK button
	if (m_okButton.getGlobalBounds().contains(mousePosXY) && m_tabletType == Tablet::Type::GIVE_BAG) {
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
		// If Bribe Money Box is selected
		if (m_BoxOutline.getPosition() == m_bribeMoneyBox.getPosition()) {
			if (m_ev.text.unicode == 8) {  // Backspace key
				if (!m_bribeMoneyInput.empty()) {
					m_bribeMoneyInput.pop_back();
				}
			}
			else if (m_ev.text.unicode >= '0' && m_ev.text.unicode <= '9') { // Only allow numbers
				m_bribeMoneyInput += static_cast<char>(m_ev.text.unicode);
			}

			// Convert string to number and enforce max value
			m_bribeMoneyAmount = (m_bribeMoneyInput.empty()) ? 0 : std::stoul(m_bribeMoneyInput);
			if (m_bribeMoneyAmount > m_playerMoney) {
				m_bribeMoneyAmount = m_playerMoney;
				m_bribeMoneyInput = std::to_string(m_playerMoney);
			}

			// Update displayed text
			m_bribeMoneyBoxText.setString(m_bribeMoneyInput.empty() ? "0" : m_bribeMoneyInput);

			centerText(m_bribeMoneyBoxText, m_bribeMoneyBox);
		}
		else if (m_BoxOutline.getPosition() == m_bribeGoodsBox.getPosition()) {
			if (m_ev.text.unicode == 8) {  // Backspace key
				if (!m_bribeGoodsAmountInput.empty()) {
					m_bribeGoodsAmountInput.pop_back();
				}
			}
			else if (m_ev.text.unicode >= '0' && m_ev.text.unicode <= '9') { // Only allow numbers
				m_bribeGoodsAmountInput += static_cast<char>(m_ev.text.unicode);
			}

			// Convert string to number and enforce max value
			m_bribeGoodsAmount = (m_bribeGoodsAmountInput.empty()) ? 0 : std::stoul(m_bribeGoodsAmountInput);
			if (m_bribeGoodsAmount > BRIBE_GOODS_AMOUNT_MAX) {
				m_bribeGoodsAmount = BRIBE_GOODS_AMOUNT_MAX;
				m_bribeGoodsAmountInput = std::to_string(m_bribeGoodsAmount);
			}

			// Update displayed text
			m_bribeGoodsBoxText.setString(m_bribeGoodsAmountInput.empty() ? "0" : m_bribeGoodsAmountInput);

			centerText(m_bribeGoodsBoxText, m_bribeGoodsBox);
		}
	}
}

void Tablet::centerText(sf::Text & text, sf::RectangleShape & button)
{
	float x = button.getPosition().x + (button.getSize().x - text.getLocalBounds().width) / 2.f;
	float y = button.getPosition().y + (button.getSize().y - text.getLocalBounds().height) / 2.f - 5.f;
	x -= text.getLocalBounds().left;
	y -= text.getLocalBounds().top / 2;
	text.setPosition(x, y);
}

sf::Text Tablet::getBribeText() const
{
	return m_bribeMoneyText;
}

unsigned int Tablet::getBribeAmount() const
{
	return m_bribeMoneyAmount;
}

unsigned int Tablet::getBribedGoodsAmount() const
{
	return m_bribeGoodsAmount;
}

Card::CardType Tablet::getPresentedGoods() const
{
	return m_selectedGoods;
}

Card::CardType Tablet::getBribedGoodsType() const
{
	return m_bribeGoodsTypeInput;
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
	m_parentWindow->draw(m_backgroundMask);

	switch (m_tabletType)
	{
	case Tablet::Type::GIVE_BAG:
		m_parentWindow->draw(m_tabletGiveBag);

		m_parentWindow->draw(m_giveBagInfoText1);
		m_parentWindow->draw(m_giveBagInfoText2);

		for (int i = 0; i < 8; i++) {
			if (i < 4) {
				m_parentWindow->draw(m_goodsButtons[i]);
			}
			m_parentWindow->draw(m_goodsBribeButtons[i]);
		}

		m_parentWindow->draw(m_bribeMoneyText);
		m_parentWindow->draw(m_bribeMoneyBox);

		m_parentWindow->draw(m_bribeGoodsText);
		m_parentWindow->draw(m_bribeGoodsBox);

		m_parentWindow->draw(m_BoxOutline);

		m_parentWindow->draw(m_bribeMoneyBoxText); // m_BoxOutline needs to be drawn before this object
		m_parentWindow->draw(m_bribeGoodsBoxText); // m_BoxOutline needs to be drawn before this object

		m_parentWindow->draw(m_maxMoneyText);

		m_parentWindow->draw(m_okButton);
		m_parentWindow->draw(m_okButtonText);

		break;

	case Tablet::Type::INFO:
		m_parentWindow->draw(m_tabletInfo);

		m_parentWindow->draw(m_playerAvatar);
		m_parentWindow->draw(m_playerAvatarFrame);
		m_parentWindow->draw(m_playerName);

		for (int i = 0; i < 8; i++) {
			m_parentWindow->draw(m_goodsButtons[i]);
			m_parentWindow->draw(m_goodsAmountText[i]);
		}

		//for (int i = 0; i < 4; i++) {
		//	m_parentWindow->draw(m_goldMedals[i]);
		//	m_parentWindow->draw(m_silverMedals[i]);
		//	m_parentWindow->draw(m_blackMarketMedals[i]);
		//}

		for (auto& medal : m_blackMarketMedals) {
			m_parentWindow->draw(medal);
		}

		m_parentWindow->draw(m_moneyIcon);
		m_parentWindow->draw(m_moneyText);

		//m_parentWindow->draw(m_scoreIcon);
		//m_parentWindow->draw(m_scoreText);

		break;

	default:
		break;
	}
}

bool Tablet::isTabletVisible() const
{
	return m_isTabletVisible;
}

void Tablet::showTablet(Tablet::Type type, int maxMoney, Player* player)
{
	m_playerMoney = maxMoney;
	m_isTabletVisible = true;
	m_tabletType = type;

	switch (type)
	{
	case Tablet::Type::GIVE_BAG:
		setupGiveBagUI();

		break;

	case Tablet::Type::INFO:
		setupInfoUI(player);

		break;

	default:
		break;
	}

}

void Tablet::hideTablet()
{
	m_isTabletVisible = false;
}

void Tablet::resetOptions()
{
	m_confirm = false;
	m_bribeMoneyAmount = 0;
	m_bribeGoodsAmount = 0;
	m_playerMoney = 0;
	m_bribeMoneyInput = "";
	m_bribeGoodsAmountInput = "";
	m_selectedGoods = Card::INVALID;
	m_bribeGoodsTypeInput = Card::INVALID;
}

void Tablet::setupGiveBagUI()
{
	// Tablet background
	m_tabletGiveBag.setSize(sf::Vector2f(1050.f, 600.f));
	m_tabletGiveBag.setFillColor(sf::Color::White);
	m_tabletGiveBag.setTexture(&m_tabletTextureGiveBag);
	m_tabletGiveBag.setPosition(435.f, 150.f);

	std::string texturePaths[8] = { "AppleReport.png", "CheeseReport.png", "BreadReport.png", "ChickenReport.png", 
		"PepperReport.png", "MeadReport.png", "SilkReport.png", "CrossbowReport.png" };
	std::string texturePathsHighlight[8] = { "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png",
		"PepperReportHighlight.png", "MeadReportHighlight.png", "SilkReportHighlight.png", "CrossbowReportHighlight.png" };

	for (int i = 0; i < 8; ++i) {
		m_goodsButtonTextures[i].loadFromFile("assets/Images/" + texturePaths[i]);
		m_goodsButtonTexturesHighLight[i].loadFromFile("assets/Images/" + texturePathsHighlight[i]);

		// Goods buttons
		if (i < 4) {
			m_goodsButtons[i].setSize(sf::Vector2f(100, 100));
			m_goodsButtons[i].setFillColor(sf::Color::White);
			m_goodsButtons[i].setTexture(&m_goodsButtonTextures[i]);
			m_goodsButtons[i].setPosition(610.f + i * 200.f, 255.f); // Position buttons in a row
		}

		// Bribe goods buttons
		m_goodsBribeButtons[i].setSize(sf::Vector2f(50, 50));
		m_goodsBribeButtons[i].setFillColor(sf::Color::White);
		m_goodsBribeButtons[i].setTexture(&m_goodsButtonTextures[i]);
		// Position buttons in 2 rows
		float posX = 1056.f + (i % 4) * 65.f;
		float posY = (i < 4) ? 502.f : 567.f;
		m_goodsBribeButtons[i].setPosition(posX, posY);
	}

	// Info texts
	m_giveBagInfoText1.setFont(m_font);
	m_giveBagInfoText1.setFillColor(sf::Color::Black);
	m_giveBagInfoText1.setString("\"I will tell the Sheriff all I have is...\"");
	m_giveBagInfoText1.setCharacterSize(32);
	m_giveBagInfoText1.setPosition(704.f, 205.f);

	m_giveBagInfoText2.setFont(m_font);
	m_giveBagInfoText2.setFillColor(sf::Color::Black);
	m_giveBagInfoText2.setString("\"To make sure I get away, I should give him...\"");
	m_giveBagInfoText2.setCharacterSize(32);
	m_giveBagInfoText2.setPosition(617.f, 375.f);

	// Bribe text
	m_bribeMoneyText.setFont(m_font);
	m_bribeMoneyText.setFillColor(sf::Color::Black);
	m_bribeMoneyText.setString("Money:");
	m_bribeMoneyText.setCharacterSize(32);
	m_bribeMoneyText.setPosition(610.f, 437.f);

	m_bribeGoodsText.setFont(m_font);
	m_bribeGoodsText.setFillColor(sf::Color::Black);
	m_bribeGoodsText.setString("Goods:");
	m_bribeGoodsText.setCharacterSize(32);
	m_bribeGoodsText.setPosition(1052.f, 437.f);

	// Bribe box
	m_bribeMoneyBox.setSize(sf::Vector2f(120.f, 50.f));
	m_bribeMoneyBox.setFillColor(sf::Color(200, 200, 200));
	m_bribeMoneyBox.setPosition(732.f, 432.f);

	m_bribeGoodsBox.setSize(sf::Vector2f(120.f, 50.f));
	m_bribeGoodsBox.setFillColor(sf::Color(200, 200, 200));
	m_bribeGoodsBox.setPosition(1172.f, 432.f);

	// Box outline
	m_BoxOutline.setSize(m_bribeMoneyBox.getSize());
	m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 0));
	m_BoxOutline.setPosition(m_bribeMoneyBox.getPosition());

	// Bribe texts inside box
	m_bribeMoneyBoxText.setFont(m_font);
	m_bribeMoneyBoxText.setCharacterSize(28);
	m_bribeMoneyBoxText.setFillColor(sf::Color::Black);
	m_bribeMoneyBoxText.setString("0");
	centerText(m_bribeMoneyBoxText, m_bribeMoneyBox);

	m_bribeGoodsBoxText.setFont(m_font);
	m_bribeGoodsBoxText.setCharacterSize(28);
	m_bribeGoodsBoxText.setFillColor(sf::Color::Black);
	m_bribeGoodsBoxText.setString("0");
	centerText(m_bribeGoodsBoxText, m_bribeGoodsBox);

	// Current money text
	m_maxMoneyText.setFont(m_font);
	m_maxMoneyText.setFillColor(sf::Color::Black);
	m_maxMoneyText.setString("Current money: " + std::to_string(m_playerMoney) + "g");
	m_maxMoneyText.setCharacterSize(28);
	m_maxMoneyText.setPosition(610.f, 497.f);

	// OK button
	m_okButton.setSize({ 160, 50 });
	m_okButton.setFillColor(sf::Color(100, 149, 237));
	m_okButton.setPosition(880.f, 630.f);

	// OK text
	m_okButtonText.setFont(m_font);
	m_okButtonText.setString("Confirm");
	m_okButtonText.setCharacterSize(28);
	centerText(m_okButtonText, m_okButton);
}

void Tablet::setupInfoUI(Player * player)
{
	// Test
	//std::cout << "Player money: " << player->getPlayerMoney() << std::endl;

	// Tablet background
	m_tabletInfo.setSize(sf::Vector2f(1110.f, 1158.f));
	m_tabletInfo.setFillColor(sf::Color::White);
	m_tabletInfo.setTexture(&m_tabletTextureInfo);
	m_tabletInfo.setPosition(405.f, -39.f);

	// Player avatar
	m_playerAvatar = player->getAvatar();
	m_playerAvatarFrame = player->getAvatarFrame();
	m_playerName = player->getNameText();
	if (player->isUserPlayer()) {
		m_playerAvatar.setPosition(710.f, 164.f);
		m_playerAvatarFrame.setPosition(690.f, 144.f);
	}
	else {
		m_playerAvatar.setPosition(730.f, 210.f);
		m_playerAvatarFrame.setPosition(710.f, 190.f);
	}

	m_playerName.setCharacterSize(30);
	m_playerName.setPosition(m_playerAvatar.getPosition() + 
		(player->isUserPlayer() ? sf::Vector2f(-10.f, 150.f) : sf::Vector2f(-10.f, 110.f)));

	// Player money
	m_moneyIcon.setSize(sf::Vector2f(70.f, 70.f));
	m_moneyIcon.setFillColor(sf::Color::White);
	m_moneyIcon.setTexture(&m_moneyIconTexture);
	m_moneyIcon.setPosition(1045.f, 154.f);
	m_moneyText.setFont(m_font);
	m_moneyText.setFillColor(sf::Color::Black);
	m_moneyText.setString(std::to_string(m_playerMoney));
	m_moneyText.setCharacterSize(42);
	m_moneyText.setPosition(1140.f, 167.f);

	// Player score
	m_playerScore = player->getPlayerScore();
	m_scoreIcon.setSize(sf::Vector2f(70.f, 70.f));
	m_scoreIcon.setFillColor(sf::Color::White);
	m_scoreIcon.setTexture(&m_scoreIconTexture);
	m_scoreIcon.setPosition(1045.f, 245.f);
	m_scoreText.setFont(m_font);
	m_scoreText.setFillColor(sf::Color::Black);
	m_scoreText.setString(std::to_string(m_playerScore));
	m_scoreText.setCharacterSize(42);
	m_scoreText.setPosition(1140.f, 255.f);
	
	// Goods
	std::string texturePathsUserHighlight[8] = 
	{ "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png", 
		"PepperReportHighlight.png", "MeadReportHighlight.png", "SilkReportHighlight.png", "CrossbowReportHighlight.png"};
	std::string texturePathsOpponentHighlight[5] = 
	{ "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png", 
		"ContrabandReport.png"};

	// Calculate contraband amount
	int contrabandAmount = 0;
	for (int i = 4; i < 8; i++) {
		contrabandAmount += player->getPlayerGoodsAmount(i + 1);
	}

	for (int i = 0; i < 8; i++) {
		// If displaying Opponent info, do not display all contraband types
		if (!player->isUserPlayer() && (i > 4)) {
			m_goodsButtons[i].setScale(0.f, 0.f);
			m_goodsAmountText[i].setScale(0.f, 0.f);
			continue;
		}

		float posX = 0.f;
		float posY = 0.f;
		if (!player->isUserPlayer()) {
			m_goodsButtonTexturesHighLight[i].loadFromFile("assets/Images/" + texturePathsOpponentHighlight[i]);
			posX = i < 4 ? (610.f + (i % 4) * 200.f) : 920.f;
			posY = i < 4 ? 423.f : 610.f;
		}
		else {
			m_goodsButtonTexturesHighLight[i].loadFromFile("assets/Images/" + texturePathsUserHighlight[i]);
			posX = 610.f + (i % 4) * 200.f;
			posY = i < 4 ? 423.f : 610.f;
		}
		m_goodsButtons[i].setSize(sf::Vector2f(100.f, 100.f));
		m_goodsButtons[i].setScale(1.f, 1.f);
		m_goodsButtons[i].setFillColor(sf::Color::White);
		m_goodsButtons[i].setTexture(&m_goodsButtonTexturesHighLight[i], true);

		m_goodsButtons[i].setPosition(posX, posY); // Position buttons in a row

		// Goods Text
		m_goodsAmountText[i].setCharacterSize(48);
		m_goodsAmountText[i].setFont(m_font);
		m_goodsAmountText[i].setFillColor(sf::Color::Black);
		m_goodsAmountText[i].setScale(1.f, 1.f);

		if (player->isUserPlayer() || (i < 4)) {
			// Get the amount of legal goods (have to shift index by 1 because first card type is INVALID)
			int goodAmount = player->getPlayerGoodsAmount(i + 1);
			std::string amount = std::to_string(goodAmount);
			m_goodsAmountText[i].setString(amount);
			m_goodsAmountText[i].setPosition(posX + (goodAmount < 10 ? 35.f : 20.f), posY + 115.f);
		}
		// If display opponent info, just display the total contraband amount
		else {
			// Display contraband amount
			std::string amount = std::to_string(contrabandAmount);
			m_goodsAmountText[i].setString(amount);
			m_goodsAmountText[i].setPosition(posX + (contrabandAmount < 10 ? 35.f : 20.f), posY + 115.f);
		}
	}

	//// Gold medals
	//for (int i = 0; i < 4; i++) {
	//	m_goldMedals[i].setSize(sf::Vector2f(70.f, 70.f));
	//	m_goldMedals[i].setTexture(&m_goldMedalTexture);
	//	m_goldMedals[i].setPosition(m_goodsButtons[i].getPosition() + sf::Vector2f(50.f, -50.f));

	//	// Only show if player gain that medal
	//	if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::GOLD) {
	//		m_goldMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
	//	}
	//	else {
	//		m_goldMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
	//	}
	//}

	//// Silver medals
	//for (int i = 0; i < 4; i++) {
	//	m_silverMedals[i].setSize(sf::Vector2f(70.f, 70.f));
	//	m_silverMedals[i].setTexture(&m_silverMedalTexture);
	//	m_silverMedals[i].setPosition(m_goodsButtons[i].getPosition() + sf::Vector2f(50.f, -50.f));

	//	// Only show if player gain that medal
	//	if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::SILVER) {
	//		m_silverMedals[i].setFillColor(sf::Color(255, 255, 255, 220));
	//	}
	//	else {
	//		m_silverMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
	//	}
	//}

	// Black market medals
	for (int i = 0; i < 6; i++) {
		m_blackMarketMedals[i].setSize(sf::Vector2f(190.f, 120.f));
		m_blackMarketMedals[i].setTexture(&m_blackMarketMedalTexture[i]);
		m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
		float posX;
		float posY;
		// Setup positions
		if (i % 2 == 0) {
			posX = 600.f + i * 132.5;
			posY = 790.f;
		}
		else {
			posX = 480.f + i * 132.5;
			posY = 820.f;
		}
		m_blackMarketMedals[i].setPosition(posX, posY);

		// Only show if player gain that medal
		if (player->getPlayerMedalStatus(i / 2 + 5) == Player::MedalStatus::BLACK_MARKET_HIGH && (i % 2 == 0)) {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else if (player->getPlayerMedalStatus(i / 2 + 5) == Player::MedalStatus::BLACK_MARKET_LOW && (i % 2 != 0)) {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else if (player->getPlayerMedalStatus(i / 2 + 5) == Player::MedalStatus::BLACK_MARKET_BOTH) {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
		}
	}
}

bool Tablet::confirmSelection()
{
	if (m_selectedGoods == Card::INVALID) {
		// Return early
		return false;
	}

	if (m_bribeGoodsAmount != 0 && m_bribeGoodsTypeInput == Card::INVALID) {
		// Return early
		return false;
	}

	if (m_bribeGoodsAmount == 0 && m_bribeGoodsTypeInput != Card::INVALID) {
		// Return early
		return false;
	}

	// Log the selection
	std::cout << "Reported goods: " << Card::m_cardNameToString.at(m_selectedGoods)
		<< ", Bribe: " << m_bribeMoneyAmount << " Gold" << std::endl;

	return true;
}