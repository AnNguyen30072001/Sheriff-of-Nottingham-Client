#include "../include/Core/Tablet.h"
#include <iostream>

Tablet::Tablet(sf::RenderWindow* parentWindow)
{
	m_bribeAmount = 0;
	m_playerMoney = 0;
	m_playerScore = 0;
	m_selectedGoods = Card::INVALID;
	m_parentWindow = parentWindow;
	m_isTabletVisible = false;
	m_confirm = false;

	if (!m_font.loadFromFile("assets/arial-font/arial.ttf")) {
		std::cerr << "Error loading tablet font!" << std::endl;
	}

	if (!m_goldMedalTexture.loadFromFile("assets/Images/GoldToken.png") || 
		!m_silverMedalTexture.loadFromFile("assets/Images/SilverToken.png") || 
		!m_blackMarketMedalTexture.loadFromFile("assets/Images/BlackMarketToken.png") ||
		!m_moneyIconTexture.loadFromFile("assets/Images/MoneyIcon.png") || 
		!m_scoreIconTexture.loadFromFile("assets/Images/ScoreIcon.png")) {
		std::cerr << "Error loading tablet texture!" << std::endl;
	}

	if (!m_tabletTextureGiveBag.loadFromFile("assets/Images/ReportBackground.png")) {
		std::cerr << "Error loading tablet background!" << std::endl;
	}

	if (!m_tabletTextureInfo.loadFromFile("assets/Images/ReportBackground_2.png")) {
		std::cerr << "Error loading tablet background!" << std::endl;
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

	if (!m_tabletInfo.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosXY)) && m_tabletType == Tablet::Type::INFO) {
		hideTablet();
		resetOptions();

		return true;
	}

	// Handle goods selection
	for (int i = 0; i < 4; ++i) {
		if (m_goodsButtons[i].getGlobalBounds().contains(mousePosXY) && m_tabletType == Tablet::Type::GIVE_BAG) {
			m_selectedGoods = static_cast<Card::CardType>(i + 1);
			// Highlight selected button
			for (int j = 0; j < 4; ++j) {
				m_goodsButtons[j].setTexture(&m_goodsButtonTextures[j]);
			}
			m_goodsButtons[i].setTexture(&m_goodsButtonTexturesHighLight[i]); // Highlight

			return true;
		}
	}

	// Click inside bribe box
	if (m_bribeBox.getGlobalBounds().contains(mousePosXY) && m_tabletType == Tablet::Type::GIVE_BAG) {
		m_BoxOutline.setFillColor(sf::Color(255, 255, 255, 255));
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
	m_parentWindow->draw(m_backgroundMask);

	switch (m_tabletType)
	{
	case Tablet::Type::GIVE_BAG:
		m_parentWindow->draw(m_tabletGiveBag);

		for (int i = 0; i < 4; i++) {
			m_parentWindow->draw(m_goodsButtons[i]);
		}
		m_parentWindow->draw(m_bribeText);
		m_parentWindow->draw(m_bribeBox);
		m_parentWindow->draw(m_BoxOutline);
		m_parentWindow->draw(m_bribeBoxText);

		m_parentWindow->draw(m_maxMoneyText);

		m_parentWindow->draw(m_okButton);
		m_parentWindow->draw(m_okButtonText);

		break;

	case Tablet::Type::INFO:
		m_parentWindow->draw(m_tabletInfo);

		m_parentWindow->draw(m_playerAvatar);
		m_parentWindow->draw(m_playerName);

		for (int i = 0; i < 8; i++) {
			m_parentWindow->draw(m_goodsButtons[i]);
			m_parentWindow->draw(m_goodsAmountText[i]);
		}

		for (int i = 0; i < 4; i++) {
			m_parentWindow->draw(m_goldMedals[i]);
			m_parentWindow->draw(m_silverMedals[i]);
			m_parentWindow->draw(m_blackMarketMedals[i]);
		}

		m_parentWindow->draw(m_moneyIcon);
		m_parentWindow->draw(m_moneyText);

		m_parentWindow->draw(m_scoreIcon);
		m_parentWindow->draw(m_scoreText);

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
	m_bribeAmount = 0;
	m_playerMoney = 0;
	m_bribeInput = "";
	m_selectedGoods = Card::INVALID;
}

void Tablet::setupGiveBagUI()
{
	// Tablet background
	m_tabletGiveBag.setSize(sf::Vector2f(750.f, 400.f));
	m_tabletGiveBag.setFillColor(sf::Color::White);
	m_tabletGiveBag.setTexture(&m_tabletTextureGiveBag);
	m_tabletGiveBag.setPosition(585.f, 150.f);

	// Goods buttons
	std::string texturePaths[4] = { "AppleReport.png", "CheeseReport.png", "BreadReport.png", "ChickenReport.png" };
	std::string texturePathsHighlight[4] = { "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png" };
	for (int i = 0; i < 4; ++i) {
		m_goodsButtonTextures[i].loadFromFile("assets/Images/" + texturePaths[i]);
		m_goodsButtonTexturesHighLight[i].loadFromFile("assets/Images/" + texturePathsHighlight[i]);
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
	m_playerName = player->getNameText();
	m_playerAvatar.setPosition(710.f, 154.f);
	m_playerName.setCharacterSize(30);
	m_playerName.setPosition(m_playerAvatar.getPosition() + 
		(player->isUserPlayer() ? sf::Vector2f(0.f, 150.f) : sf::Vector2f(0.f, 130.f)));

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
	std::string texturePathsHighlight[8] = 
	{ "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png", 
		"PepperReportHighlight.png", "MeadReportHighlight.png", "SilkReportHighlight.png", "CrossbowReportHighlight.png"};
	for (int i = 0; i < 8; ++i) {
		m_goodsButtonTexturesHighLight[i].loadFromFile("assets/Images/" + texturePathsHighlight[i]);
		m_goodsButtons[i].setSize(sf::Vector2f(100.f, 100.f));
		m_goodsButtons[i].setFillColor(sf::Color::White);
		m_goodsButtons[i].setTexture(&m_goodsButtonTexturesHighLight[i]);
		float posX = 610.f + (i % 4) * 200.f;
		float posY = i < 4 ? 423.f : 744.f;
		m_goodsButtons[i].setPosition(posX, posY); // Position buttons in a row

		// Goods Text
		m_goodsAmountText[i].setFont(m_font);
		m_goodsAmountText[i].setFillColor(sf::Color::Black);
		// Get the amount of that card type (have to shift index by 1 because first card type is INVALID)
		int goodAmount = player->getPlayerGoodsAmount(i + 1);
		std::string amount = std::to_string(goodAmount);
		m_goodsAmountText[i].setString(amount);
		m_goodsAmountText[i].setCharacterSize(48);
		m_goodsAmountText[i].setPosition(posX + (goodAmount < 10 ? 35.f : 20.f), posY + 116.f);
	}

	// Gold medals
	for (int i = 0; i < 4; i++) {
		m_goldMedals[i].setSize(sf::Vector2f(70.f, 70.f));
		m_goldMedals[i].setTexture(&m_goldMedalTexture);
		m_goldMedals[i].setPosition(m_goodsButtons[i].getPosition() + sf::Vector2f(50.f, -50.f));

		// Only show if player gain that medal
		if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::GOLD) {
			m_goldMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else {
			m_goldMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
		}
	}

	// Silver medals
	for (int i = 0; i < 4; i++) {
		m_silverMedals[i].setSize(sf::Vector2f(70.f, 70.f));
		m_silverMedals[i].setTexture(&m_silverMedalTexture);
		m_silverMedals[i].setPosition(m_goodsButtons[i].getPosition() + sf::Vector2f(50.f, -50.f));

		// Only show if player gain that medal
		if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::SILVER) {
			m_silverMedals[i].setFillColor(sf::Color(255, 255, 255, 220));
		}
		else {
			m_silverMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
		}
	}

	// Black market medals
	for (int i = 0; i < 4; i++) {
		m_blackMarketMedals[i].setSize(sf::Vector2f(70.f, 70.f));
		m_blackMarketMedals[i].setTexture(&m_blackMarketMedalTexture);
		m_blackMarketMedals[i].setPosition(m_goodsButtons[i + 4].getPosition() + sf::Vector2f(50.f, -50.f));

		// Only show if player gain that medal
		if (player->getPlayerMedalStatus(i + 5) == Player::MedalStatus::BLACK_MARKET) {
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

	// Process the selection
	std::cout << "Reported goods: " << Card::m_cardNameToString.at(m_selectedGoods)
		<< ", Bribe: " << m_bribeAmount << " Gold" << std::endl;

	return true;
}