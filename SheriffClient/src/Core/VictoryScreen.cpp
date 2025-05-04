#include "../include/Core/VictoryScreen.h"
#include "../include/Entities/Card.h"
#include "../include/Core/GameState.h"
#include <iostream>

Summary::Summary(std::vector<Player*> playerList)
{
	initVariables(playerList);
	initWindow();
}

Summary::~Summary()
{
	delete this->m_window;

	for (auto& player : m_playerList) {
		delete player;
	}
}

const bool Summary::running() const
{
	return false;
}

bool Summary::pollEvents()
{
	while (m_window->pollEvent(m_ev)) {
		sf::Vector2i mousePosXYLocal = sf::Mouse::getPosition(*m_window);
		sf::Vector2f mousePosXY = m_window->mapPixelToCoords(mousePosXYLocal);
		switch (m_ev.type) {
		case sf::Event::Closed:
			m_window->close();
			break;

		case sf::Event::MouseButtonPressed:
			if (m_ev.mouseButton.button == sf::Mouse::Left) {
				handleMouseClick(mousePosXY);
			}
			break;

		case sf::Event::MouseMoved:
			handleMouseHover(mousePosXY);

			break;

		default:
			break;
		}
	}

	return true;
}

bool Summary::update()
{
	float deltaTime = m_clock.restart().asSeconds();
	m_animationPlayer.update(deltaTime);

	pollEvents();

	return true;
}

bool Summary::render()
{
	m_window->clear();

	m_window->draw(m_background);

	// Draw players avatar, name
	for (int i = 0; i < m_playerList.size(); i++) {
		m_window->draw(m_playerList[i]->getAvatar());
		m_window->draw(m_playerList[i]->getAvatarFrame());
		if (i == m_highlightedPlayerIdx) {
			m_window->draw(m_playerList[i]->getNameText());
		}
	}

	// Draw info
	m_window->draw(m_scoreText);
	m_window->draw(m_moneyIcon);
	m_window->draw(m_moneyText);

	for (int i = 0; i < GOODS_ICON_NUM_MAX; i++) {
		m_window->draw(m_goodsIcon[i]);
		m_window->draw(m_goodsAmountText[i]);
	}

	for (int i = 0; i < LEGAL_GOODS_TOKEN_NUM_MAX; i++) {
		m_window->draw(m_tokenGoldIcon[i]);
		m_window->draw(m_tokenSilverIcon[i]);
	}

	for (int i = 0; i < BLACK_MARKET_MEDAL_NUM_MAX; i++) {
		m_window->draw(m_blackMarketMedals[i]);
	}

	m_window->draw(m_buttonFinish);

	m_window->display();

	return true;
}

void Summary::initVariables(std::vector<Player*> playerList)
{
	m_playerList = playerList;
	m_highlightedPlayerIdx = 0;

	if (!m_font.loadFromFile("assets/arial-font/arial.ttf")) {
		std::cerr << "Error loading font!\n";
	}

	if (!m_backgroundTexture.loadFromFile("assets/Images/Background_demo.png")) {
		std::cerr << "Error loading background!\n";
	}

	if (!m_moneyIconTexture.loadFromFile("assets/Images/Tokens/MoneyIcon.png")) {
		std::cerr << "Error loading money icon!\n";
	}

	if (!m_buttonFinishTexture.loadFromFile("assets/Images/Buttons/ButtonFinish.png")) {
		std::cerr << "Error loading money icon!\n";
	}

	//m_background.setTexture(&m_backgroundTexture);
	m_background.setFillColor(sf::Color::White);

	m_scoreText.setFont(m_font);
	m_scoreText.setFillColor(sf::Color::Black);
	m_moneyText.setFont(m_font);
	m_moneyText.setFillColor(sf::Color::Black);

	std::string texturePaths[GOODS_ICON_NUM_MAX] =
	{ "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png",
		"PepperReportHighlight.png", "MeadReportHighlight.png", "SilkReportHighlight.png", "CrossbowReportHighlight.png" };

	for (int i = 0; i < GOODS_ICON_NUM_MAX; i++) {
		m_goodsIconTexture[i].loadFromFile("assets/Images/" + texturePaths[i]);
	}

	std::string tokenGoldTexturePaths[LEGAL_GOODS_TOKEN_NUM_MAX] =
	{ "GoldToken_20.png", "GoldToken_15.png", "GoldToken_15.png", "GoldToken_10.png" };
	std::string tokenSilverTexturePaths[LEGAL_GOODS_TOKEN_NUM_MAX] =
	{ "SilverToken_10.png", "SilverToken_10.png", "SilverToken_10.png", "SilverToken_5.png" };

	for (int i = 0; i < LEGAL_GOODS_TOKEN_NUM_MAX; i++) {
		m_tokenGoldIconTexture[i].loadFromFile("assets/Images/Tokens/" + tokenGoldTexturePaths[i]);
		m_tokenSilverIconTexture[i].loadFromFile("assets/Images/Tokens/" + tokenSilverTexturePaths[i]);

		m_tokenGoldIcon[i].setTexture(&m_tokenGoldIconTexture[i]);
		m_tokenSilverIcon[i].setTexture(&m_tokenSilverIconTexture[i]);
	}

	for (auto& text : m_goodsAmountText) {
		text.setFont(m_font);
		text.setFillColor(sf::Color::Black);
	}

	std::string blackMedalTexturePaths[BLACK_MARKET_MEDAL_NUM_MAX] = {
	"PepperToken_v1.png", "PepperToken_v2.png",
	"MeadToken_v1.png", "MeadToken_v2.png",
	"SilkToken_v1.png", "SilkToken_v2.png"
	};
	for (int i = 0; i < BLACK_MARKET_MEDAL_NUM_MAX; i++) {
		if (!m_blackMarketMedalTexture[i].loadFromFile("assets/Images/Tokens/" + blackMedalTexturePaths[i])) {
			std::cerr << "Error loading medal texture!" << std::endl;
		}
	}

	m_moneyIcon.setTexture(&m_moneyIconTexture);
	for (int i = 0; i < GOODS_ICON_NUM_MAX; i++) {
		m_goodsIcon[i].setTexture(&m_goodsIconTexture[i]);
	}
	for (int i = 0; i < BLACK_MARKET_MEDAL_NUM_MAX; i++) {
		m_blackMarketMedals[i].setTexture(&m_blackMarketMedalTexture[i]);
	}

	m_buttonFinish.setTexture(&m_buttonFinishTexture);
}

void Summary::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);

	// Backgrounds
	m_background.setPosition(sf::Vector2f(0.f, 0.f));
	m_background.setSize(sf::Vector2f(1920.f, 1080.f));

	// Finish button
	m_buttonFinish.setFillColor(sf::Color::White);
	m_buttonFinish.setSize(sf::Vector2f(300.f, 80.f));
	m_buttonFinish.setPosition(810.f, 975.f);
	
	// Ranking players in descending order
	updatePlayersMedalStatus();
	for (auto& player : m_playerList) {
		updatePlayerScore(player);
	}
	std::sort(m_playerList.begin(), m_playerList.end(), [](Player* player1, Player* player2) 
		{
		return player1->getPlayerScore() > player2->getPlayerScore();
		}
	);

	// Players layout
	setPlayersLayout();
	highlightPlayer(m_highlightedPlayerIdx);

	// Player info
	setPlayerInfoLayout(m_highlightedPlayerIdx);
}

bool Summary::handleMouseClick(sf::Vector2f mousePosXY)
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (m_playerList[i]->getAvatarFrame().getGlobalBounds().contains(mousePosXY)) {
			// Play a sound
			m_soundPlayer.play("ButtonPressed.wav", Sound::Type::EFFECT, 0.85);

			// Update info of highlighted player
			m_highlightedPlayerIdx = i;

			setPlayersLayout();
			setPlayerInfoLayout(m_highlightedPlayerIdx);

			highlightPlayer(m_highlightedPlayerIdx);

			return true;
		}
	}

	if (m_buttonFinish.getGlobalBounds().contains(mousePosXY)) {
		// Reset game state, back to login menu
		g_gameState = LOGIN_VIEW;
	}

	return false;
}

bool Summary::handleMouseHover(sf::Vector2f mousePosXY)
{
	return false;
}

void Summary::setPlayersLayout()
{
	int lobbySize = m_playerList.size();

	float posXOffset = 145.f + 60.f;
	float distX = (1655.f - 145.f) / (lobbySize - 1);
	float posY = 94.f + 60.f;

	for (int i = 0; i < lobbySize; i++) {
		m_playerList[i]->getAvatar().setRadius(60.f);
		m_playerList[i]->getAvatar().setOrigin(60.f, 60.f);
		m_playerList[i]->getAvatarFrame().setRadius(80.f);
		m_playerList[i]->getAvatarFrame().setOrigin(80.f, 80.f);

		if (i == m_highlightedPlayerIdx) {
			m_playerList[i]->getAvatar().setFillColor(sf::Color(255, 255, 255, 255));
			m_playerList[i]->getAvatarFrame().setFillColor(sf::Color(255, 255, 255, 255));
		}
		else {
			m_playerList[i]->getAvatar().setFillColor(sf::Color(255, 255, 255, 120));
			m_playerList[i]->getAvatarFrame().setFillColor(sf::Color(255, 255, 255, 120));
		}

		m_playerList[i]->getNameText().setCharacterSize(48);
		m_playerList[i]->getNameText().setFillColor(sf::Color::Black);
		m_playerList[i]->getNameText().setOutlineThickness(0.f);
		m_nameTextBox.setSize(sf::Vector2f(640.f, 80.f));

		m_playerList[i]->getAvatar().setPosition(posXOffset + i * distX, posY);
		m_playerList[i]->getAvatarFrame().setPosition(m_playerList[i]->getAvatar().getPosition());
		m_nameTextBox.setPosition(640.f, 295.f);
		centerText(m_playerList[i]->getNameText(), m_nameTextBox);
	}
}

void Summary::setPlayerInfoLayout(int index)
{
	// Initial position properties

	float posXOffset = 169.f;
	float distX = 400.f;
	float posYLineFirst = 491.f;
	float posYLineSecond = 595.f;

	float posXOffsetBlackToken = 161.f;
	float distXBlackToken = 664.f;
	float posYBlackTokenLineFirst = 700.f;
	float posYBlackTokenLineSecond = 750.f;

	// Set properties

	m_moneyIcon.setSize(sf::Vector2f(54.f, 54.f));
	m_moneyIcon.setFillColor(sf::Color::White);
	m_moneyIcon.setPosition(1543.f, 382.f);

	m_scoreTextBox.setSize(sf::Vector2f(260.f, 80.f));
	m_scoreTextBox.setPosition(830.f, 365.f);
	m_scoreText.setCharacterSize(60);
	m_scoreText.setString(std::to_string(m_playerList[index]->getPlayerScore()));
	centerText(m_scoreText, m_scoreTextBox);

	m_moneyTextBox.setSize(sf::Vector2f(140.f, 54.f));
	m_moneyTextBox.setPosition(1613.f, 382.f);
	m_moneyText.setCharacterSize(40);
	m_moneyText.setString(std::to_string(m_playerList[index]->getPlayerMoney()));
	centerText(m_moneyText, m_moneyTextBox);

	// Goods info
	for (int i = 0; i < GOODS_ICON_NUM_MAX; i++) {
		m_goodsIcon[i].setSize(sf::Vector2f(64.f, 64.f));
		m_goodsIcon[i].setFillColor(sf::Color::White);
		m_goodsIcon[i].setPosition(posXOffset + (i % 4) * distX, (i < 4) ? posYLineFirst : posYLineSecond);

		sf::Vector2f refPos = m_goodsIcon[i].getPosition();

		m_goodsAmountTextBox[i].setSize(sf::Vector2f(228.f, 42.f));
		m_goodsAmountTextBox[i].setPosition(refPos + sf::Vector2f(113.f, 11.f));
		m_goodsAmountText[i].setCharacterSize(32);
		m_goodsAmountText[i].setString(std::to_string(m_playerList[index]->getPlayerGoodsAmount(i + 1)));
		centerText(m_goodsAmountText[i], m_goodsAmountTextBox[i]);
	}

	// Gold and silver token info
	for (int i = 0; i < LEGAL_GOODS_TOKEN_NUM_MAX; i++) {
		m_tokenGoldIcon[i].setSize(sf::Vector2f(42.f, 42.f));
		m_tokenSilverIcon[i].setSize(sf::Vector2f(42.f, 42.f));

		m_tokenGoldIcon[i].setPosition(m_goodsIcon[i].getPosition() + sf::Vector2f(41.f, -6.f));
		m_tokenSilverIcon[i].setPosition(m_goodsIcon[i].getPosition() + sf::Vector2f(41.f, -6.f));

		if (m_playerList[index]->getPlayerMedalStatus(i + 1) == Player::MedalStatus::GOLD) {
			m_tokenGoldIcon[i].setFillColor(sf::Color(255, 255, 255, 255));
			m_tokenSilverIcon[i].setFillColor(sf::Color(255, 255, 255, 0));
		}

		else if (m_playerList[index]->getPlayerMedalStatus(i + 1) == Player::MedalStatus::SILVER) {
			m_tokenGoldIcon[i].setFillColor(sf::Color(255, 255, 255, 0));
			m_tokenSilverIcon[i].setFillColor(sf::Color(255, 255, 255, 255));
		}

		else {
			m_tokenGoldIcon[i].setFillColor(sf::Color(255, 255, 255, 0));
			m_tokenSilverIcon[i].setFillColor(sf::Color(255, 255, 255, 0));
		}
	}

	// Black market token info
	for (int i = 0; i < BLACK_MARKET_MEDAL_NUM_MAX; i++) {
		m_blackMarketMedals[i].setSize(sf::Vector2f(265.f, 167.f));
		m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 0));

		float posX = posXOffsetBlackToken + (i / 2) * distXBlackToken;
		float posY = (i % 2 == 0) ? posYBlackTokenLineFirst : posYBlackTokenLineSecond;

		m_blackMarketMedals[i].setPosition(posX, posY);

		// Only show if player gain that medal
		if (m_playerList[index]->getPlayerMedalStatus(i / 2 + 5) == Player::MedalStatus::BLACK_MARKET_HIGH && (i % 2 == 0)) {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else if (m_playerList[index]->getPlayerMedalStatus(i / 2 + 5) == Player::MedalStatus::BLACK_MARKET_LOW && (i % 2 != 0)) {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else if (m_playerList[index]->getPlayerMedalStatus(i / 2 + 5) == Player::MedalStatus::BLACK_MARKET_BOTH) {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 255));
		}
		else {
			m_blackMarketMedals[i].setFillColor(sf::Color(255, 255, 255, 0));
		}
	}
}

void Summary::updatePlayersMedalStatus()
{
	for (int i = 0; i < 4; i++)
	{
		int kingCount = 0;  /* Most card player have */
		int queenCount = 0; /* Second most card player have */

		int numberOfKing = 0;  /* number of player have the most card */
		int numberOfQueen = 0; /* number of player have the second most card */

		for (auto& player : m_playerList)
		{	
			int goodAmount = player->getPlayerGoodsAmount(i + 1);
			if (goodAmount > kingCount)
			{
				queenCount = kingCount;
				numberOfQueen = numberOfKing;
				kingCount = goodAmount;
				numberOfKing = 1;
			}
			else if (goodAmount == kingCount)
			{
				numberOfKing++;
			}
			else if (goodAmount > queenCount)
			{
				queenCount = goodAmount;
				numberOfQueen = 1;
			}
			else if (goodAmount == queenCount)
			{
				numberOfQueen++;
			}
		}

		/* No one have this card */
		if (kingCount == 0)
		{
			continue;
		}

		if (numberOfKing > 1) /* >= 2 King */
		{
			int sharedPrice = (Card::cardTypeToGoldBonus.at(static_cast<Card::CardType>(i + 1)) + Card::cardTypeToSilverBonus.at(static_cast<Card::CardType>(i + 1))) / numberOfKing;
			for (auto& player : m_playerList)
			{
				int goodAmount = player->getPlayerGoodsAmount(i + 1);
				if (goodAmount == kingCount)
				{
					player->setPlayerScore(sharedPrice);
					player->setPlayerMedalStatus(i + 1, Player::MedalStatus::GOLD);
				}
			}
		}
		else /* 1 King */
		{
			for (auto& player : m_playerList)
			{
				int goodAmount = player->getPlayerGoodsAmount(i + 1);
				if (goodAmount == kingCount)
				{
					player->setPlayerScore(Card::cardTypeToGoldBonus.at(static_cast<Card::CardType>(i + 1)));
					player->setPlayerMedalStatus(i + 1, Player::MedalStatus::GOLD);
				}
			}
			if (queenCount > 1) /* >= 2 Queen */
			{
				int sharedPrice = Card::cardTypeToSilverBonus.at(static_cast<Card::CardType>(i + 1)) / numberOfQueen;
				for (auto& player : m_playerList)
				{
					int goodAmount = player->getPlayerGoodsAmount(i + 1);
					if (goodAmount == queenCount)
					{
						player->setPlayerScore(sharedPrice);
						player->setPlayerMedalStatus(i + 1, Player::MedalStatus::SILVER);
					}
				}
			}
			else /* 1 Queen */
			{
				for (auto& player : m_playerList)
				{
					int goodAmount = player->getPlayerGoodsAmount(i + 1);
					if (goodAmount == queenCount && queenCount > 0)
					{
						player->setPlayerScore(Card::cardTypeToSilverBonus.at(static_cast<Card::CardType>(i + 1)));
						player->setPlayerMedalStatus(i + 1, Player::MedalStatus::SILVER);
					}
				}
			}
		}
	}
}

void Summary::updatePlayerScore(Player * player)
{
		int bonusLegalScore = player->getPlayerScore();
		int finalScore = 0;
		// Convert money to score
		finalScore += player->getPlayerMoney();
	
		// Convert goods value to score
		for (int i = 0; i < 8; i++) {
			int goodAmount = player->getPlayerGoodsAmount(i + 1);
			// Multiply amount with value of card type, then add to score
			finalScore += goodAmount * Card::cardTypeToValue.at(static_cast<Card::CardType>(i + 1));
		}
	
		// Convert medals to score
		for (int i = 0; i < 8; i++) {
			/* Move this section to updatePlayersMedalStatus() function */

			//// Gold bonus
			//if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::GOLD) {
			//	finalScore += Card::cardTypeToGoldBonus.at(static_cast<Card::CardType>(i + 1));
			//}
			//// Silver bonus
			//else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::SILVER) {
			//	finalScore += Card::cardTypeToSilverBonus.at(static_cast<Card::CardType>(i + 1));
			//}

			// Black market low bonus
			if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::BLACK_MARKET_LOW) {
				finalScore += Card::cardTypeToContrabandBonusLow.at(static_cast<Card::CardType>(i + 1));
			}
			// Black market high bonus
			else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::BLACK_MARKET_HIGH) {
				finalScore += Card::cardTypeToContrabandBonusHigh.at(static_cast<Card::CardType>(i + 1));
			}
			// Black market high bonus and low bonus
			else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::BLACK_MARKET_BOTH) {
				finalScore += Card::cardTypeToContrabandBonusHigh.at(static_cast<Card::CardType>(i + 1));
				finalScore += Card::cardTypeToContrabandBonusLow.at(static_cast<Card::CardType>(i + 1));
			}
		}
	
		// Set player score
		player->setPlayerScore(bonusLegalScore + finalScore);
}

void Summary::highlightPlayer(int index)
{
	for (int i = 0; i < m_playerList.size(); i++) {
		if (i == index) {
			m_animationPlayer.addAnimation(new Animation(m_playerList[i]->getAvatar(), Animation::Type::SCALE, 1.2, 0.2));
			m_animationPlayer.addAnimation(new Animation(m_playerList[i]->getAvatarFrame(), Animation::Type::SCALE, 1.2, 0.2));
		}

		else if (m_playerList[i]->getAvatarFrame().getScale() != sf::Vector2f(1.f, 1.f)) {
			m_animationPlayer.addAnimation(new Animation(m_playerList[i]->getAvatar(), Animation::Type::SCALE, 1.f, 0.2));
			m_animationPlayer.addAnimation(new Animation(m_playerList[i]->getAvatarFrame(), Animation::Type::SCALE, 1.f, 0.2));
		}
	}
}

void Summary::centerText(sf::Text & text, sf::RectangleShape & boundingBox)
{
	float x = boundingBox.getPosition().x + (boundingBox.getSize().x - text.getLocalBounds().width) / 2.f;
	float y = boundingBox.getPosition().y + (boundingBox.getSize().y - text.getLocalBounds().height) / 2.f - 5.f;
	x -= text.getLocalBounds().left;
	y -= text.getLocalBounds().top / 2;
	text.setPosition(x, y);
}

