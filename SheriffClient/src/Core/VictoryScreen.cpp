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
	return false;
}

bool Summary::update()
{


	return true;
}

bool Summary::render()
{
	m_window->clear();

	m_window->draw(m_background);

	// Draw players avatar, name
	for (auto& player : m_playerList) {
		m_window->draw(player->getAvatar());
		m_window->draw(player->getAvatarFrame());
		m_window->draw(player->getNameText());
	}

	// Draw info
	m_window->draw(m_scoreText);
	m_window->draw(m_moneyIcon);
	m_window->draw(m_moneyText);

	for (int i = 0; i < GOODS_ICON_NUM_MAX; i++) {
		m_window->draw(m_goodsIcon[i]);
		m_window->draw(m_goodsAmountText[i]);
	}

	for (int i = 0; i < BLACK_MARKET_MEDAL_NUM_MAX; i++) {
		m_window->draw(m_blackMarketMedals[i]);
	}

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

	m_scoreText.setFont(m_font);
	m_moneyText.setFont(m_font);

	std::string texturePaths[8] =
	{ "AppleReportHighlight.png", "CheeseReportHighlight.png", "BreadReportHighlight.png", "ChickenReportHighlight.png",
		"PepperReportHighlight.png", "MeadReportHighlight.png", "SilkReportHighlight.png", "CrossbowReportHighlight.png" };

	for (int i = 0; i < GOODS_ICON_NUM_MAX; i++) {
		m_goodsIconTexture[i].loadFromFile("assets/Images/" + texturePaths[i]);
	}

	for (auto& text : m_goodsAmountText) {
		text.setFont(m_font);
	}

	std::string blackMedalTexturePaths[6] = {
	"PepperToken_v1.png", "PepperToken_v2.png",
	"MeadToken_v1.png", "MeadToken_v2.png",
	"SilkToken_v1.png", "SilkToken_v2.png"
	};
	for (int i = 0; i < 6; i++) {
		if (!m_blackMarketMedalTexture[i].loadFromFile("assets/Images/Tokens/" + blackMedalTexturePaths[i])) {
			std::cerr << "Error loading meddal texture!" << std::endl;
		}
	}
}

void Summary::initWindow()
{
	m_videoMode.width = 1920;
	m_videoMode.height = 1080;
	m_window = new sf::RenderWindow(m_videoMode, "Sheriff of Nottingham", sf::Style::Default);
	m_window->setVerticalSyncEnabled(true);
	
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


}

bool Summary::handleMouseClick(sf::Vector2f mousePosXY)
{
	return false;
}

void Summary::setPlayersLayout()
{
	int lobbySize = m_playerList.size();

	float posXOffset = 124.f;
	float distX = (1634.f - 124.f) / (lobbySize - 1);
	float posY = 72.f;

	for (int i = 0; i < lobbySize; i++) {
		m_playerList[i]->getAvatar().setRadius(80.f);
		m_playerList[i]->getAvatarFrame().setRadius(100.f);
		m_playerList[i]->getNameText().setCharacterSize(48);

		m_playerList[i]->getAvatar().setPosition(posXOffset + i * distX, posY);
		m_playerList[i]->getAvatarFrame().setPosition(m_playerList[i]->getAvatar().getPosition() + sf::Vector2f(-30.f, -60.f));
		m_playerList[i]->getNameText().setPosition(890.f, 305.f);
	}
}

void Summary::setPlayerInfoLayout(int index)
{
	float posXOffset = 169.f;
	float distX = 400.f;
	float posYLineFirst = 491.f;
	float posYLineSecond = 595.f;

	float posXOffsetBlackToken = 161.f;
	float distXBlackToken = 664.f;
	float posYBlackTokenLineFirst = 700.f;
	float posYBlackTokenLineSecond = 750.f;
}

void Summary::updatePlayersMedalStatus()
{
	// Find out each good amount to be considered gold or silver
	int goldMedalAmount[4] = { 0, 0, 0, 0 };
	int silverMedalAmount[4] = { 0, 0, 0, 0 };
	int playerGoodAmount = 0;
	for (auto& player : m_playerList) {
		for (int i = 0; i < 4; i++) {
			playerGoodAmount = player->getPlayerGoodsAmount(i + 1);
			if (playerGoodAmount > goldMedalAmount[i]) {
				silverMedalAmount[i] = goldMedalAmount[i];
				goldMedalAmount[i] = playerGoodAmount;
			}
			else if (playerGoodAmount < goldMedalAmount[i] && playerGoodAmount > silverMedalAmount[i]) {
				silverMedalAmount[i] = playerGoodAmount;
			}
		}
	}
	
	// Give medal to players according to good amount
	for (auto& player : m_playerList) {
		for (int i = 0; i < 4; i++) {
			playerGoodAmount = player->getPlayerGoodsAmount(i + 1);
			if (playerGoodAmount == goldMedalAmount[i] && goldMedalAmount[i] != 0) {
				player->setPlayerMedalStatus(i + 1, Player::MedalStatus::GOLD);
			}
			else if (playerGoodAmount == silverMedalAmount[i] && silverMedalAmount[i] != 0) {
				player->setPlayerMedalStatus(i + 1, Player::MedalStatus::SILVER);
			}
			else {
				player->setPlayerMedalStatus(i + 1, Player::MedalStatus::NONE);
			}
		}
	}
}

void Summary::updatePlayerScore(Player * player)
{
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
			// Gold bonus
			if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::GOLD) {
				finalScore += Card::cardTypeToGoldBonus.at(static_cast<Card::CardType>(i + 1));
			}
			// Silver bonus
			else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::SILVER) {
				finalScore += Card::cardTypeToSilverBonus.at(static_cast<Card::CardType>(i + 1));
			}
			// Black market low bonus
			else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::BLACK_MARKET_LOW) {
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
		player->setPlayerScore(finalScore);
}

void Summary::centerText(sf::Text & text, sf::RectangleShape & boundingBox)
{
	float x = boundingBox.getPosition().x + (boundingBox.getSize().x - text.getLocalBounds().width) / 2.f;
	float y = boundingBox.getPosition().y + (boundingBox.getSize().y - text.getLocalBounds().height) / 2.f - 5.f;
	x -= text.getLocalBounds().left;
	y -= text.getLocalBounds().top / 2;
	text.setPosition(x, y);
}

