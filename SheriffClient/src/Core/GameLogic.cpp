#include "../include/Core/GameLogic.h"
#include <random>
#include <ctime>

GameLogic::GameLogic(Game* game) : m_game(game)
{
	m_bribeAmount = 0;
	m_goodsReport = Card::UNKNOWN;
}

//void GameLogic::updatePlayersMedalStatus()
//{
//	//// Find out each good amount to be considered gold or silver
//	int goldMedalAmount[4] = { 0, 0, 0, 0 };
//	int silverMedalAmount[4] = { 0, 0, 0, 0 };
//	int playerGoodAmount = 0;
//	for (auto& player : m_game->m_playerList) {
//		for (int i = 0; i < 4; i++) {
//			playerGoodAmount = player->getPlayerGoodsAmount(i + 1);
//			if (playerGoodAmount > goldMedalAmount[i]) {
//				silverMedalAmount[i] = goldMedalAmount[i];
//				goldMedalAmount[i] = playerGoodAmount;
//			}
//			else if (playerGoodAmount < goldMedalAmount[i] && playerGoodAmount > silverMedalAmount[i]) {
//				silverMedalAmount[i] = playerGoodAmount;
//			}
//		}
//	}
//
//	//// Give medal to players according to good amount
//	for (auto& player : m_game->m_playerList) {
//		for (int i = 0; i < 4; i++) {
//			playerGoodAmount = player->getPlayerGoodsAmount(i + 1);
//			if (playerGoodAmount == goldMedalAmount[i] && goldMedalAmount[i] != 0) {
//				player->setPlayerMedalStatus(i + 1, Player::MedalStatus::GOLD);
//			}
//			else if (playerGoodAmount == silverMedalAmount[i] && silverMedalAmount[i] != 0) {
//				player->setPlayerMedalStatus(i + 1, Player::MedalStatus::SILVER);
//			}
//			else {
//				player->setPlayerMedalStatus(i + 1, Player::MedalStatus::NONE);
//			}
//		}
//
//		for (int j = 4; j < 8; j++) {
//			playerGoodAmount = player->getPlayerGoodsAmount(j + 1);
//			if (playerGoodAmount >= 3) {
//				player->setPlayerMedalStatus(j + 1, Player::MedalStatus::BLACK_MARKET);
//			}
//		}
//	}
//}
//
//void GameLogic::updatePlayerScore(Player * player)
//{
//	int finalScore = 0;
//
//	// Convert money to score
//	finalScore += player->getPlayerMoney();
//
//	// Convert goods value to score
//	for (int i = 0; i < 8; i++) {
//		int goodAmount = player->getPlayerGoodsAmount(i + 1);
//		// Multiply amount with value of card type, then add to score
//		finalScore += goodAmount * Card::cardTypeToValue.at(static_cast<Card::CardType>(i + 1));
//	}
//
//	// Convert medals to score
//	for (int i = 0; i < 8; i++) {
//		// Gold bonus
//		if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::GOLD) {
//			finalScore += Card::cardTypeToGoldBonus.at(static_cast<Card::CardType>(i + 1));
//		}
//		// Silver bonus
//		else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::SILVER) {
//			finalScore += Card::cardTypeToSilverBonus.at(static_cast<Card::CardType>(i + 1));
//		}
//		// Black market bonus
//		else if (player->getPlayerMedalStatus(i + 1) == Player::MedalStatus::BLACK_MARKET) {
//			finalScore += Card::cardTypeToContrabandBonus.at(static_cast<Card::CardType>(i + 1));
//		}
//	}
//
//	// Set player score
//	player->setPlayerScore(finalScore);
//}

bool GameLogic::attemptReconnect(float deltaTime)
{
	m_game->m_elapsedTime += deltaTime;
	// Attempt reconnect every 5 seconds
	if (m_game->m_elapsedTime < 5.f) {
		return false;
	}
	// Attempt connection to server
	if (!Network::getInstance().connect()) {
		m_game->m_elapsedTime = 0.f;
		return false;
	}

	// Connection success.
	else {
		Network::getInstance().startListening();
		Network::getInstance().startProcessingMessageQueue();
		return true;
	}
}

void GameLogic::handleDealCardsEvent(const nlohmann::json & jsonMessage)
{
	int index = 0;

	// Show all current cards in hand
	for (auto& card : m_game->m_userHand) {
		std::lock_guard<std::mutex> lock(m_game->m_userHandMutex);
		card->getCard().setScale(1.f, 1.f);
	}

	// Get all Card types
	for (auto it = jsonMessage["Cards"].begin(); it != jsonMessage["Cards"].end(); it++) {
		if (m_game->m_userHand.size() >= 6U) break;	// Limit guard
		// Get card type as string
		std::string cardName = *it;

		// Add to user hand
		m_game->addToUserHand(Card::m_stringToCardName.at(cardName));

		// Add drawing animation of that card from the main deck
		std::lock_guard<std::mutex> lock(m_game->m_userHandMutex);
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setPosition(m_game->m_deck->getMainDeck().getPosition() + sf::Vector2f(75, 100));
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setScale(sf::Vector2f(0.f, 0.f));

		float posX = 520.f + ((m_game->m_userHand.size() - 1) % 6) * 150.f;
		float delay = index * 0.3;
		// Add animation, after animation done, send response message
		if (m_game->m_userHand.size() == 6U) {
			m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
				0.1, sf::Vector2f(posX, 635.f), 1.f, delay, [this, jsonMessage]
			{ Network::getInstance().respondMessage(jsonMessage); }));
		}
		else {
			m_game->m_userHand[m_game->m_userHand.size() - 1]->animationMove(0.1, sf::Vector2f(posX, 635), 1.f, delay);
		}
		index++;
	}
}

void GameLogic::handleStartTurnEvent(std::string playerName)
{
	for (int i = 0; i < m_game->m_playerList.size(); i++) {
		// Reset turn UI
		m_game->m_playerList[i]->setTurn(false);
		// Handle logic if find player's turn
		if (m_game->m_playerList[i]->getPlayerName() == playerName) {
			// Hide tablet if needed
			m_game->m_tablet->hideTablet();

			m_game->m_playerList[i]->setTurn(true);

			// Start the timer if user player
			if (m_game->m_playerList[i]->isUserPlayer()) {
				m_game->m_timer->reset();
				m_game->m_timer->start();
			}
			else {
				m_game->m_timer->stop();
			}

			// If it is sheriff's turn, setup animations and UI of giving bag
			if (m_game->m_playerList[i]->isSheriff() && !m_game->m_dummyCards.empty()) {
				// Change game state
				m_game->m_gameEvent = Game::SHERIFF_TURN;
				// Setup initial position to make sure the cards are centered
				float totalWidth = m_game->m_dummyCards.size() * 135.f + (m_game->m_dummyCards.size() - 1) * 15.f;
				float posXOffset = ((1920 - totalWidth) / 2.f) + 67.5;
				// Animation
				for (int i = 0; i < m_game->m_dummyCards.size(); i++) {
					float posX = posXOffset + i * (135.f + 15.f);
					std::lock_guard<std::mutex> lock(m_game->m_dummyCardsMutex);
					m_game->m_dummyCards[i]->getCard().setOrigin(67.5, 97.5);
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_dummyCards[i]->getCard(), Animation::Type::MOVE_AND_SCALE, 0.3,
						sf::Vector2f(posX, 395.f), 1.f));
				}

				// Set infomation
				m_game->m_goodsReportIcon.setPosition(sf::Vector2f(920.f, 180.f));
				if (m_goodsReport == Card::CHEESE) {
					m_game->m_goodsReportIcon.setPosition(m_game->m_goodsReportIcon.getPosition() + sf::Vector2f(0.f, 10.f));
				}

				m_game->m_textMutex.lock();
				m_game->m_goodsReportText.setString("\"Just " + std::to_string(m_game->m_dummyCards.size()) + "         , I swear!\"");
				m_game->m_goodsReportTexture.loadFromFile("assets/Images/" + Card::m_cardNameToString.at(m_goodsReport) + "ReportHighlight.png");
				m_game->m_goodsReportIcon.setTexture(&m_game->m_goodsReportTexture, true);
				m_game->m_infoText.setString("Bribe:");
				m_game->m_infoText.setPosition(800.f, 540.f);
				m_game->m_moneyIcon.setPosition(950.f, 520.f);
				m_game->m_moneyIcon.setScale(1.f, 1.f);
				m_game->m_moneyText.setString(std::to_string(m_bribeAmount));
				m_game->m_moneyText.setFillColor(sf::Color::White);
				m_game->m_moneyText.setCharacterSize(48);
				m_game->m_moneyText.setScale(1.f, 1.f);
				m_game->m_moneyText.setPosition(1070.f, 540.f);
				m_game->m_textMutex.unlock();

				// Temporary hide all decks and user cards
				//m_game->m_deck->getDiscardDeckLeft().setScale(0.f, 0.f);
				//m_game->m_deck->getDiscardDeckRight().setScale(0.f, 0.f);
				//m_game->m_deck->getMainDeck().setScale(0.f, 0.f);
				for (auto& card : m_game->m_userHand) {
					std::lock_guard<std::mutex> lock(m_game->m_userHandMutex);
					card->getCard().setScale(0.f, 0.f);
				}
			}
			// If it is a merchant's turn, reset all previous stored goods infomation, for safety measure
			else {
				m_game->m_tablet->resetOptions();
				m_game->m_dummyCards.clear();
				m_game->m_selectedCards.clear();

				m_game->m_discardDone = false;
				m_game->m_gameEvent = Game::DEFAULT;

				// Show decks and user cards again
				//m_game->m_deck->getDiscardDeckLeft().setScale(1.f, 1.f);
				//m_game->m_deck->getDiscardDeckRight().setScale(1.f, 1.f);
				//m_game->m_deck->getMainDeck().setScale(1.f, 1.f);
				for (auto& card : m_game->m_userHand) {
					std::lock_guard<std::mutex> lock(m_game->m_userHandMutex);
					card->getCard().setScale(1.f, 1.f);
				}
			}
		}
	}
}

void GameLogic::handlePresentEvent()
{
	// Get infomation
	//std::map<std::string, int> cardCount;
	//for (auto& userCard : m_game->m_userHand) {
	//	if (userCard->isSelected()) {
	//		cardCount[Card::m_cardNameToString.at(userCard->getCardType())]++;
	//	}
	//}

	//for (auto it = cardCount.begin(); it != cardCount.end(); ++it) {
	//	message["Bag"][it->first] = it->second;
	//}

	// Prepare JSON message
	json message;
	message["MessageType"] = "MERCHANT_GIVE_BAG";
	message["PlayerName"] = m_game->m_playerList[0]->getPlayerName();
	message["Report"] = Card::m_cardNameToString.at(m_game->m_tablet->getPresentedGoods());
	message["Fee"] = std::to_string(m_game->m_tablet->getBribeAmount());

	for (auto& userCard : m_game->m_userHand) {
		if (userCard->isSelected()) {
			message["Bag"].push_back(Card::m_cardNameToString.at(userCard->getCardType()));
		}
	}

	// Convert message and send
	std::string messageString = message.dump();
	Network::sendMessage(messageString);

	// Save information for next Sheriff turn
	m_goodsReport = m_game->m_tablet->getPresentedGoods();
	m_bribeAmount = m_game->m_tablet->getBribeAmount();

	// Reset tablet and wait for server response
	m_game->m_tablet->resetOptions();
	m_game->m_gameEvent = Game::IDLE;
}

void GameLogic::handleGiveBagEvent(const nlohmann::json & jsonMessage)
{
	// Move presented cards from user hand to present bag
	for (int i = 0; i < m_game->m_userHand.size(); i++) {
		if (m_game->m_userHand[i]->isSelected()) {
			m_game->m_selectedCardsMutex.lock();
			m_game->m_selectedCards.push_back(m_game->m_userHand[i]);
			m_game->m_selectedCardsMutex.unlock();
			m_game->removeFromUserHand(i);
			i--;
			// Push a dummy card to render next Sheriff turn
			m_game->m_dummyCardsMutex.lock();
			m_game->m_dummyCards.push_back(new Card(Card::UNKNOWN));
			m_game->m_dummyCardsMutex.unlock();
			m_game->m_dummyCards[m_game->m_dummyCards.size() - 1]->getCard().setScale(0.f, 0.f);
			m_game->m_dummyCards[m_game->m_dummyCards.size() - 1]->getCard().setPosition(m_game->m_playerList[0]->getBagIcon().getPosition() + sf::Vector2f(40.f, 40.f));
		}
	}
	// Animation highlighting bag
	m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_playerList[0]->getBagIcon(), Animation::Type::SCALE, 1.2, 0.25, 0.2,
		[this, jsonMessage]
	{
		m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_playerList[0]->getBagIcon(), Animation::Type::SCALE, 1.f, 0.25, 0.f,
			[this, jsonMessage]
		{
			// Send a response message
			Network::getInstance().respondMessage(jsonMessage);
		}));
	}));
	for (auto& card : m_game->m_selectedCards) {
		// Animation put cards into bag
		m_game->m_animationPlayer.addAnimation(new Animation(card->getCard(), Animation::Type::MOVE_AND_SCALE, 0.3f,
			m_game->m_playerList[0]->getBagIcon().getPosition() + sf::Vector2f(40.f, 40.f), 0.f));
	}
	// Store information of merchant giving bag for next Sheriff turn
	m_game->m_MerchantShowingBagIndex = 0;
	// Hide tablet
	m_game->m_tablet->hideTablet();
	// Re-arrange userhand
	m_game->userHandUI();
	m_game->m_gameEvent = Game::DEFAULT;
}

void GameLogic::handleOpponentGiveBagEvent(const nlohmann::json & jsonMessage)
{
	for (int i = 0; i < m_game->m_playerList.size(); i++) {
		if (m_game->m_playerList[i]->getPlayerName() == jsonMessage["PlayerName"]) {
			// Clear dummy cards to store new ones. Just a safety measure
			m_game->m_dummyCardsMutex.lock();
			for (auto& card : m_game->m_dummyCards) {
				delete card;
			}
			m_game->m_dummyCards.clear();
			m_game->m_dummyCardsMutex.unlock();

			// Store data to render the next Sheriff turn
			m_goodsReport = Card::m_stringToCardName.at(jsonMessage["Report"]);
			std::string bribe = jsonMessage["Fee"];
			m_bribeAmount = std::stoi(bribe);
			std::string amount = jsonMessage["Amount"];
			int goodsAmount = std::stoi(amount);
			m_game->m_MerchantShowingBagIndex = i;
			std::cout << "Merchant idx: " << m_game->m_MerchantShowingBagIndex << std::endl;

			// Add animation put cards to bag
			for (int j = 0; j < goodsAmount; j++) {
				m_game->m_dummyCardsMutex.lock();
				m_game->m_dummyCards.push_back(new Card(Card::UNKNOWN));
				m_game->m_dummyCardsMutex.unlock();
			}
			for (int j = 0; j < m_game->m_dummyCards.size(); j++) {
				m_game->m_dummyCards[j]->getCard().setScale(0.4, 0.4);
				m_game->m_dummyCards[j]->getCard().setPosition(m_game->m_playerList[i]->getAvatar().getPosition() + sf::Vector2f(30.f, 30.f));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_dummyCards[j]->getCard(), Animation::Type::MOVE_AND_SCALE, 0.5,
					m_game->m_playerList[i]->getBagIcon().getPosition() + sf::Vector2f(30.f, 30.f), 0.f));
			}
			// Animation highlighting bag
			m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_playerList[i]->getBagIcon(), Animation::Type::SCALE, 1.2, 0.2, 0.3,
				[this, jsonMessage, i]
			{
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_playerList[i]->getBagIcon(), Animation::Type::SCALE, 1.f, 0.2, 0.f,
					[this, jsonMessage]
				{
					// Send a response message
					Network::getInstance().respondMessage(jsonMessage);
				}));
			}));
			break;
		}
	}
}

void GameLogic::setupExchangeEvent()
{
	// Move presented cards from user hand to discard area
	for (int i = 0; i < m_game->m_userHand.size(); i++) {
		if (m_game->m_userHand[i]->isSelected()) {
			m_game->m_selectedCardsMutex.lock();
			m_game->m_selectedCards.push_back(m_game->m_userHand[i]);
			m_game->m_selectedCardsMutex.unlock();

			m_game->removeFromUserHand(i);
			i--;
		}
	}

	// Move the selected cards to temporary slots
	for (int i = 0; i < m_game->m_selectedCards.size(); i++) {
		float posX = 78.f + (i % 3) * 105.f;
		float posY = i < 3 ? 350.f : 505.f;
		m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_selectedCards[i]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.3, sf::Vector2f(posX, posY), 0.667));
	}
	// Re-arrange userhand
	m_game->userHandUI();

	// Position guide text
	m_game->m_textMutex.lock();
	m_game->m_guideText.setString("Draw new cards from any deck");
	m_game->m_guideText.setPosition(668.f, 180.f);
	m_game->m_textMutex.unlock();

	// Change state machine
	m_game->m_gameEvent = Game::WITHDRAW;
}

void GameLogic::setupOpponentExchangeEvent(int playerIndex, std::vector<std::string> cardNames)
{
	sf::Vector2f startPos = m_game->m_playerList[playerIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f);
	sf::Vector2f startScale = sf::Vector2f(0.f, 0.f);
	float endScale = 0.667;
	Card* opponentSelectedCard = nullptr;

	// Push the cards into m_selectedCards
	for (int i = 0; i < cardNames.size(); i++) {
		opponentSelectedCard = new Card(Card::m_stringToCardName.at(cardNames[i]));

		m_game->m_selectedCardsMutex.lock();
		m_game->m_selectedCards.push_back(opponentSelectedCard);
		m_game->m_selectedCardsMutex.unlock();

		opponentSelectedCard->getCard().setScale(startScale);
		opponentSelectedCard->getCard().setPosition(startPos);
		// Move selected card into temporary slots
		float posX = 78.f + (i % 3) * 105.f;
		float posY = i < 3 ? 350.f : 505.f;
		m_game->m_animationPlayer.addAnimation(new Animation(opponentSelectedCard->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.3, sf::Vector2f(posX, posY), endScale));
	}
}

void GameLogic::handleWithdrawEvent(Game::PileType type)
{
	Card* topCard = nullptr;
	float posX = 0.f;
	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		if (m_game->m_deck->getStackLeft().empty()) return;			// Prevent drawing from null

		topCard = m_game->m_deck->getStackLeft().top();
		// Add new card to user hand and start animation
		//m_game->addToUserHand(topCard->getCardType());
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(650.f, 324.f));
		posX = 520.f + ((m_game->m_userHand.size() - 1) % 6) * 150.f;
		m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.2, sf::Vector2f(posX, 635.f)));

		// Pop the left deck
		m_game->m_deck->getStackLeft().pop();
		delete topCard;
		if (!m_game->m_deck->getStackLeft().empty()) {
			m_game->m_deck->setDiscardDeckLeftTexture(m_game->m_deck->getStackLeft().top()->getCardType());
		}
		else {
			m_game->m_deck->setDiscardDeckLeftOutOfStockTexture();
		}

		break;
	case Game::PileType::RIGHT_DISCARD_PILE:
		if (m_game->m_deck->getStackRight().empty()) return;			// Prevent drawing from null

		topCard = m_game->m_deck->getStackRight().top();
		// Add new card to user hand and start animation
		//m_game->addToUserHand(topCard->getCardType());
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(1162.f, 324.f));
		posX = 520.f + ((m_game->m_userHand.size() - 1) % 6) * 150.f;
		m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.2, sf::Vector2f(posX, 635.f)));

		// Pop the right deck
		m_game->m_deck->getStackRight().pop();
		delete topCard;
		if (!m_game->m_deck->getStackRight().empty()) {
			m_game->m_deck->setDiscardDeckRightTexture(m_game->m_deck->getStackRight().top()->getCardType());
		}
		else {
			m_game->m_deck->setDiscardDeckRightOutOfStockTexture();
		}

		break;
	case Game::PileType::MAIN_DECK:
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setScale(0.8, 0.8);
		m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard().setPosition(sf::Vector2f(885.f, 280.f));
		posX = 520.f + ((m_game->m_userHand.size() - 1) % 6) * 150.f;
		m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_userHand[m_game->m_userHand.size() - 1]->getCard(), Animation::Type::MOVE_AND_SCALE,
			0.2, sf::Vector2f(posX, 635.f)));

		break;
	default:
		break;
	}
}

void GameLogic::handleDiscardEvent(Game::PileType type, std::string cardName)
{
	// Only discard once per turn
	m_game->m_discardDone = true;

	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		for (int i = 0; i < m_game->m_selectedCards.size(); i++) {
			// Look for the right card to insert to pile
			if ((m_game->m_selectedCards[i]->isDragging() && m_game->m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName))
				|| (m_game->m_gameEvent == Game::TIMEOUT_DISCARD && m_game->m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName))) {
				m_game->m_deck->getStackLeft().push(m_game->m_selectedCards[i]);
				// Animation card fit into deck, callback to function set texture
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_selectedCards[i]->getCard(), Animation::Type::MOVE,
					0.2, sf::Vector2f(650.f, 324.f), 0.8, 0.f, [this]
				{m_game->getDeck()->setDiscardDeckLeftTexture(this->m_game->m_deck->getStackLeft().top()->getCardType()); }));
				// Animation card fit into deck, callback to remove from selected cards
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_selectedCards[i]->getCard(), Animation::Type::SCALE,
					0.8, 0.2, 0.f, [this, i]
				{
					m_game->getSelectedCards().erase(m_game->getSelectedCards().begin() + i);
					if (m_game->m_gameEvent == Game::TIMEOUT_DISCARD) m_game->m_gameEvent = Game::TIMEOUT_DISCARD;
					else m_game->m_gameEvent = Game::DISCARD;
				}));

				// Reset status
				m_game->m_anyCardDragged = false;

				break;
			}
		}

		break;

	case Game::PileType::RIGHT_DISCARD_PILE:
		for (int i = 0; i < m_game->m_selectedCards.size(); i++) {
			// Look for the right card to insert to pile
			if ((m_game->m_selectedCards[i]->isDragging() && m_game->m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName))
				|| (m_game->m_gameEvent == Game::TIMEOUT_DISCARD && m_game->m_selectedCards[i]->getCardType() == Card::m_stringToCardName.at(cardName))) {
				m_game->m_deck->getStackRight().push(m_game->m_selectedCards[i]);
				// Animation card fit into deck, callback to function set texture
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_selectedCards[i]->getCard(), Animation::Type::MOVE,
					0.2, sf::Vector2f(1162.f, 324.f), 0.8, 0.f, [this]
				{m_game->getDeck()->setDiscardDeckRightTexture(this->m_game->m_deck->getStackRight().top()->getCardType()); }));
				// Animation card fit into deck, callback to remove from selected cards
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_selectedCards[i]->getCard(), Animation::Type::SCALE,
					0.8, 0.2, 0.f, [this, i]
				{
					m_game->getSelectedCards().erase(m_game->getSelectedCards().begin() + i);
					if (m_game->m_gameEvent == Game::TIMEOUT_DISCARD) m_game->m_gameEvent = Game::TIMEOUT_DISCARD;
					else m_game->m_gameEvent = Game::DISCARD;
				}));

				// Reset status
				m_game->m_anyCardDragged = false;

				break;
			}
		}

		break;

	default:
		break;
	}
}

void GameLogic::handleOpponentWithdrawEvent(Game::PileType type, int playerIndex)
{
	Card* dummyCard = nullptr;
	sf::Vector2f endPos = m_game->m_playerList[playerIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f);
	int index = 0;
	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		if (m_game->m_deck->getStackLeft().empty()) return;			// Prevent drawing from null

		m_game->m_dummyCardsMutex.lock();
		m_game->m_dummyCards.push_back(m_game->m_deck->getStackLeft().top());
		m_game->m_dummyCardsMutex.unlock();

		index = m_game->m_dummyCards.size() - 1;
		dummyCard = m_game->m_dummyCards[index];
		dummyCard->getCard().setPosition(650.f, 324.f);
		// Add animation withdraw for the player, pop the left deck and set new texture once animation is done
		m_game->m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 0.7,
			endPos, 0.6, 0.f, [this, dummyCard, index]
		{
			delete dummyCard;
			if (!m_game->m_dummyCards.empty()) { 
				m_game->m_dummyCards.erase(m_game->m_dummyCards.begin()); 
			}
		}));

		m_game->m_deck->getStackLeft().pop();
		if (!m_game->m_deck->getStackLeft().empty()) {
			m_game->m_deck->setDiscardDeckLeftTexture(m_game->m_deck->getStackLeft().top()->getCardType());
		}
		else {
			m_game->m_deck->setDiscardDeckLeftOutOfStockTexture();
		}

		break;
	case Game::PileType::RIGHT_DISCARD_PILE:
		if (m_game->m_deck->getStackRight().empty()) return;			// Prevent drawing from null

		m_game->m_dummyCardsMutex.lock();
		m_game->m_dummyCards.push_back(m_game->m_deck->getStackRight().top());
		m_game->m_dummyCardsMutex.unlock();

		index = m_game->m_dummyCards.size() - 1;
		dummyCard = m_game->m_dummyCards[index];
		dummyCard->getCard().setPosition(1162.f, 324.f);
		// Add animation withdraw for the player, pop the left deck and set new texture once animation is done
		m_game->m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 0.7,
			endPos, 0.6, 0.f, [this, dummyCard, index]
		{
			delete dummyCard;
			if (!m_game->m_dummyCards.empty()) { 
				m_game->m_dummyCards.erase(m_game->m_dummyCards.begin()); 
			}
		}));

		m_game->m_deck->getStackRight().pop();
		if (!m_game->m_deck->getStackRight().empty()) {
			m_game->m_deck->setDiscardDeckRightTexture(m_game->m_deck->getStackRight().top()->getCardType());
		}
		else {
			m_game->m_deck->setDiscardDeckRightOutOfStockTexture();
		}

		break;
	case Game::PileType::MAIN_DECK:
		// Add animation withdraw of an unknown card
		m_game->m_dummyCardsMutex.lock();
		m_game->m_dummyCards.push_back(new Card(Card::UNKNOWN));
		m_game->m_dummyCardsMutex.unlock();

		index = m_game->m_dummyCards.size() - 1;
		dummyCard = m_game->m_dummyCards[index];
		dummyCard->getCard().setPosition(885.f, 280.f);
		dummyCard->getCard().setScale(1.f, 1.f);
		m_game->m_animationPlayer.addAnimation(new Animation(dummyCard->getCard(), Animation::Type::MOVE_AND_SCALE, 0.7,
			endPos, 0.6, 0.f, [this, dummyCard, index]
		{
			delete dummyCard;
			if (!m_game->m_dummyCards.empty()) { 
				m_game->m_dummyCards.erase(m_game->m_dummyCards.begin()); 
			}
		}));

		break;

	default:
		break;
	}
}

void GameLogic::handleOpponentDiscardEvent(Game::PileType type, int playerIndex, Card::CardType cardType)
{
	sf::Vector2f startPos = sf::Vector2f(0.f, 0.f);;
	sf::Vector2f startScale = sf::Vector2f(0.f, 0.f);;
	sf::Vector2f endPosLeft = sf::Vector2f(650.f, 324.f);
	sf::Vector2f endPosRight = sf::Vector2f(1162.f, 324.f);
	Card* opponentSelectedCard = nullptr;
	int index = 0;
	for (int i = 0; i < m_game->m_selectedCards.size(); i++) {
		if (m_game->m_selectedCards[i]->getCardType() == cardType) {
			startPos = m_game->m_selectedCards[i]->getCard().getPosition();
			startScale = m_game->m_selectedCards[i]->getCard().getScale();
			m_game->m_selectedCards.erase(m_game->m_selectedCards.begin() + i);
			break;
		}
	}

	switch (type)
	{
	case Game::PileType::LEFT_DISCARD_PILE:
		m_game->m_deck->getStackLeft().push(new Card(cardType));
		m_game->m_dummyCards.push_back(m_game->m_deck->getStackLeft().top());
		index = m_game->m_dummyCards.size() - 1;
		opponentSelectedCard = m_game->m_dummyCards[index];
		opponentSelectedCard->getCard().setPosition(startPos);
		opponentSelectedCard->getCard().setScale(startScale);

		// Add animation discard card
		m_game->m_animationPlayer.addAnimation(new Animation(opponentSelectedCard->getCard(), Animation::Type::MOVE_AND_SCALE, 0.5,
			endPosLeft, 0.8, 0.f, [this, index]
		{
			m_game->m_deck->setDiscardDeckLeftTexture(m_game->m_deck->getStackLeft().top()->getCardType());

			if (!m_game->m_dummyCards.empty()) {
				m_game->m_dummyCards.erase(m_game->m_dummyCards.begin());
			}
		}));

		break;
	case Game::PileType::RIGHT_DISCARD_PILE:
		//m_game->m_deck->getStackRight().push(new Card(cardType));
		//m_game->m_dummyCards.push_back(m_game->m_deck->getStackRight().top());
		//index = m_game->m_dummyCards.size() - 1;
		//dummyCard = m_game->m_dummyCards[index];
		//dummyCard->getCard().setPosition(startPos);
		//dummyCard->getCard().setScale(startScale);
		m_game->m_deck->getStackRight().push(new Card(cardType));
		m_game->m_dummyCards.push_back(m_game->m_deck->getStackRight().top());
		index = m_game->m_dummyCards.size() - 1;
		opponentSelectedCard = m_game->m_dummyCards[index];
		opponentSelectedCard->getCard().setPosition(startPos);
		opponentSelectedCard->getCard().setScale(startScale);

		// Add animation discard card
		m_game->m_animationPlayer.addAnimation(new Animation(opponentSelectedCard->getCard(), Animation::Type::MOVE_AND_SCALE, 0.5,
			endPosRight, 0.8, 0.f, [this, index]
		{
			m_game->m_deck->setDiscardDeckRightTexture(m_game->m_deck->getStackRight().top()->getCardType());

			if (!m_game->m_dummyCards.empty()) {
				m_game->m_dummyCards.erase(m_game->m_dummyCards.begin());
			}
		}));

		break;

	default:
		break;
	}
}

void GameLogic::handleSheriffInspectEvent(const nlohmann::json & jsonMessage)
{
	m_game->m_gameEvent = Game::REVEAL;
	// Setup
	int revealedIndex = 0;
	bool isIllegal = false;
	std::vector<Card::CardType> cardTypes;
	for (auto it = jsonMessage["Bag"].begin(); it != jsonMessage["Bag"].end(); it++) {
		std::string cardName = *it;
		Card::CardType cardType = Card::m_stringToCardName.at(cardName);
		cardTypes.push_back(cardType);
		// If the bag contain any illegal good, save this information
		if (cardType != m_goodsReport) {
			isIllegal = true;
		}
	}

	Player* sheriffPlayer = nullptr;
	for (auto& player : m_game->m_playerList) {
		if (player->getPlayerName() == jsonMessage["PlayerName"]) {
			sheriffPlayer = player;
		}
	}
	// Add animation Merchant retrieve bribe
	m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.5,
		m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f));
	m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.5,
		m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f, [this, sheriffPlayer, cardTypes, revealedIndex, jsonMessage, isIllegal]
	{
		m_game->m_moneyText.setCharacterSize(30);
		// Recursively reveal cards, until all cards are revealed
		std::cout << "Game Event: " << m_game->getGameEvent() << std::endl;
		revealCard(sheriffPlayer, cardTypes, revealedIndex, jsonMessage, isIllegal);
	}));
}

void GameLogic::handleSheriffPassEvent(const nlohmann::json & jsonMessage)
{
	m_game->m_gameEvent = Game::REVEAL;
	// Setup
	int revealedIndex = 0;
	std::vector<Card::CardType> cardTypes;
	for (auto it = jsonMessage["Bag"].begin(); it != jsonMessage["Bag"].end(); it++) {
		std::string cardName = *it;
		Card::CardType cardType = Card::m_stringToCardName.at(cardName);
		cardTypes.push_back(cardType);
	}

	Player* sheriffPlayer = nullptr;
	for (auto& player : m_game->m_playerList) {
		if (player->getPlayerName() == jsonMessage["PlayerName"]) {
			sheriffPlayer = player;
		}
	}

	// Add animation Sheriff receive bribe
	m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.5,
		sheriffPlayer->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f));
	m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.5,
		sheriffPlayer->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 1.f, [this, sheriffPlayer, cardTypes, revealedIndex, jsonMessage]
	{
		m_game->m_moneyText.setCharacterSize(30);
		// Update sheriff and merchant catalog
		m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->setPlayerMoney(m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getPlayerMoney() - m_bribeAmount);
		sheriffPlayer->setPlayerMoney(sheriffPlayer->getPlayerMoney() + m_bribeAmount);
		std::cout << "Game Event: " << m_game->getGameEvent() << std::endl;
		// Recursively reveal cards, until all cards are revealed
		revealCard(sheriffPlayer, cardTypes, revealedIndex, jsonMessage, false);
	}));
}

void GameLogic::revealCard(Player * sheriff, std::vector<Card::CardType> cardTypes, int revealIndex, const nlohmann::json& jsonMessage, bool isIllegal)
{
	// Add animation to reveal a card
	m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_dummyCards[revealIndex]->getCard(), Animation::Type::SCALE, 1.2, 0.2, 0.2, [this, cardTypes, sheriff, revealIndex, jsonMessage, isIllegal]
	{
		// Reveal the card
		m_game->m_dummyCards[revealIndex]->setCardTexture(cardTypes[revealIndex]);
		m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_dummyCards[revealIndex]->getCard(), Animation::Type::SCALE, 1.0, 0.2, 0.f, [this, cardTypes, sheriff, revealIndex, jsonMessage, isIllegal]
		{
			sf::Vector2f posOffset = m_game->m_dummyCards[revealIndex]->getCard().getPosition();

			// If Sheriff chose to Pass, or if Sheriff inspects the illegal bag but the current card is legal, merchant get the goods
			if (jsonMessage["MessageType"] == "SHERIFF_PASS_RESPONSE" || 
				(cardTypes[revealIndex] == m_goodsReport && isIllegal) ) {
				std::cout << "Legal\n";

				// Update player's catalog
				m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->increasePlayerGoodsAmount(cardTypes[revealIndex], 1);

				// Continue revealing until all cards are revealed
				int nextIndex = revealIndex + 1;
				if (nextIndex < cardTypes.size()) {
					revealCard(sheriff, cardTypes, nextIndex, jsonMessage, isIllegal);
				}
				else {
					// Update players medal status
					//updatePlayersMedalStatus();

					// Update players score
					//m_game->m_textMutex.lock();
					//for (auto& player : m_game->m_playerList) {
					//	updatePlayerScore(player);
					//}
					//m_game->m_textMutex.unlock();

					// Retrieve the cards laid on table
					retrieveCards(jsonMessage);
				}
			}

			// If Sheriff inspect and the whole bag is legal, merchant get the cash
			else if (cardTypes[revealIndex] == m_goodsReport && !isIllegal) {
				std::cout << "Legal\n";

				// Add animation Merchant receive the cash
				int depositCash = Card::cardTypeToPenalty.at(cardTypes[revealIndex]);
				m_game->m_moneyText.setString(std::to_string(depositCash));
				m_game->m_moneyText.setScale(1.f, 1.f);
				m_game->m_moneyText.setFillColor(sf::Color::Green);
				m_game->m_moneyIcon.setScale(0.6, 0.6);
				m_game->m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
				m_game->m_moneyText.setPosition(m_game->m_moneyIcon.getPosition() + sf::Vector2f(20.f, 60.f));

				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE, 0.15,
					m_game->m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE, 0.15,
					m_game->m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.2,
					m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
					m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5, [this, cardTypes, sheriff, revealIndex, jsonMessage, posOffset, isIllegal]
				{
					// Update player's catalog
					m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->increasePlayerGoodsAmount(cardTypes[revealIndex], 1);
					m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->setPlayerMoney(m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getPlayerMoney()
						+ Card::cardTypeToPenalty.at(cardTypes[revealIndex]));

					// Add animation Sheriff got deducted
					int penalty = Card::cardTypeToPenalty.at(cardTypes[revealIndex]);
					m_game->m_moneyText.setString("-" + std::to_string(penalty));
					m_game->m_moneyText.setScale(1.f, 1.f);
					m_game->m_moneyText.setFillColor(sf::Color::Red);
					m_game->m_moneyIcon.setScale(0.6, 0.6);
					m_game->m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
					m_game->m_moneyText.setPosition(m_game->m_moneyIcon.getPosition() + sf::Vector2f(15.f, 60.f));

					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE, 0.15,
						m_game->m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE, 0.15,
						m_game->m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.3,
						sheriff->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5));
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
						sheriff->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5, [this, cardTypes, sheriff, revealIndex, jsonMessage, isIllegal]
					{
						// Update player's catalog
						sheriff->setPlayerMoney(sheriff->getPlayerMoney()
							- Card::cardTypeToPenalty.at(cardTypes[revealIndex]));

						// Continue revealing until all cards are revealed
						int nextIndex = revealIndex + 1;
						if (nextIndex < cardTypes.size()) {
							revealCard(sheriff, cardTypes, nextIndex, jsonMessage, isIllegal);
						}
						else {
							// Update players medal status
							//updatePlayersMedalStatus();

							// Update players score
							//m_game->m_textMutex.lock();
							//for (auto& player : m_game->m_playerList) {
							//	updatePlayerScore(player);
							//}
							//m_game->m_textMutex.unlock();

							// Retrieve the cards laid on table
							retrieveCards(jsonMessage);
						}
					}));
				}));
			}

			// If Sheriff inspect and the card is not legal, sheriff get the cash
			else {
				std::cout << "Not legal\n";
				// Add animation Sheriff receive the cash
				int depositCash = Card::cardTypeToPenalty.at(cardTypes[revealIndex]);
				m_game->m_moneyText.setString(std::to_string(depositCash));
				m_game->m_moneyText.setScale(1.f, 1.f);
				m_game->m_moneyText.setFillColor(sf::Color::Green);
				m_game->m_moneyIcon.setScale(0.6, 0.6);
				m_game->m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
				m_game->m_moneyText.setPosition(m_game->m_moneyIcon.getPosition() + sf::Vector2f(20.f, 60.f));

				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE, 0.15,
					m_game->m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE, 0.15,
					m_game->m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.2,
					sheriff->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5));
				m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
					sheriff->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5, [this, cardTypes, sheriff, revealIndex, jsonMessage, posOffset, isIllegal]
				{
					// Update player's catalog
					sheriff->setPlayerMoney(sheriff->getPlayerMoney()
						+ Card::cardTypeToPenalty.at(cardTypes[revealIndex]));

					// Add animation Merchant got deducted
					int penalty = Card::cardTypeToPenalty.at(cardTypes[revealIndex]);
					m_game->m_moneyText.setString("-" + std::to_string(penalty));
					m_game->m_moneyText.setScale(1.f, 1.f);
					m_game->m_moneyText.setFillColor(sf::Color::Red);
					m_game->m_moneyIcon.setScale(0.6, 0.6);
					m_game->m_moneyIcon.setPosition(posOffset + sf::Vector2f(-30.f, 120.f));
					m_game->m_moneyText.setPosition(m_game->m_moneyIcon.getPosition() + sf::Vector2f(15.f, 60.f));

					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE, 0.15,
						m_game->m_moneyText.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE, 0.15,
						m_game->m_moneyIcon.getPosition() + sf::Vector2f(0.f, 20.f), 0.6, 0.2));
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyText, Animation::Type::MOVE_AND_SCALE, 0.3,
						m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5));
					m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_moneyIcon, Animation::Type::MOVE_AND_SCALE, 0.3,
						m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.5, [this, cardTypes, sheriff, revealIndex, jsonMessage, isIllegal]
					{
						// Update player's catalog
						m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->setPlayerMoney(m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getPlayerMoney()
							- Card::cardTypeToPenalty.at(cardTypes[revealIndex]));

						// Continue revealing until all cards are revealed
						int nextIndex = revealIndex + 1;
						if (nextIndex < cardTypes.size()) {
							revealCard(sheriff, cardTypes, nextIndex, jsonMessage, isIllegal);
						}
						else {
							// Update players medal status
							//updatePlayersMedalStatus();

							// Update players score
							//m_game->m_textMutex.lock();
							//for (auto& player : m_game->m_playerList) {
							//	updatePlayerScore(player);
							//}
							//m_game->m_textMutex.unlock();

							// Retrieve the cards laid on table
							retrieveCards(jsonMessage);
						}
					}));
				}));
			}
		}));
	}));
}

void GameLogic::retrieveCards(const nlohmann::json& jsonMessage)
{
	if (m_game->m_dummyCards.empty()) return;

	for (int i = 0; i < m_game->m_dummyCards.size(); i++) {
		// If card is legal, hand it to merchant
		if (m_game->m_dummyCards[i]->getCardType() == m_goodsReport || jsonMessage["MessageType"] == "SHERIFF_PASS_RESPONSE") {
			m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_dummyCards[i]->getCard(), Animation::Type::MOVE_AND_SCALE, 0.4,
				m_game->m_playerList[m_game->m_MerchantShowingBagIndex]->getAvatar().getPosition() + sf::Vector2f(50.f, 50.f), 0.f, 0.6, [this, i, jsonMessage]
			{
				// If done process the last card
				if (i == m_game->m_dummyCards.size() - 1) {
					// Reveal process is complete
					m_game->m_revealingDone = true;

					for (Card* card : m_game->m_dummyCards) {
						delete card;
						card = nullptr;
					}
					m_game->m_dummyCards.clear();

					// Send response message
					Network::getInstance().respondMessage(jsonMessage);
				}
			}));
		}
		// If card is not legal, shuffle it back to main deck
		else {
			m_game->m_animationPlayer.addAnimation(new Animation(m_game->m_dummyCards[i]->getCard(), Animation::Type::SCALE, 0.f, 0.3, 0.6, [this, i, jsonMessage]
			{
				// If done process the last card
				if (i == m_game->m_dummyCards.size() - 1) {
					// Reveal process is complete
					m_game->m_revealingDone = true;

					for (Card* card : m_game->m_dummyCards) {
						delete card;
						card = nullptr;
					}
					m_game->m_dummyCards.clear();

					// Send response message
					Network::getInstance().respondMessage(jsonMessage);
				}
			}));
		}
	}
}

void GameLogic::handleTimeoutWithdraw()
{
	m_game->m_gameEvent = Game::TIMEOUT_WITHDRAW;
	//int leftPileCardsNumber = m_game->m_deck->getStackLeft().size();
	//int rightPileCardsNumber = m_game->m_deck->getStackRight().size();
	//int leftDrawCount = 0;
	//int rightDrawCount = 0;
	//int middleDrawCount = 0;
	int cardsToDraw = 6 - m_game->m_userHand.size();
	for (int i = 0; i < cardsToDraw; i++) {
		json message;
		message["MessageType"] = "MERCHANT_WITHDRAW_CARDS_TIMEOUT";
		message["PlayerName"] = m_game->m_playerList[0]->getPlayerName();
		message["Pile"] = "MAIN_DECK";
		std::string messageString = message.dump();
		Network::getInstance().sendMessage(messageString);
	}
}

void GameLogic::handleTimeoutDiscard()
{
	m_game->m_gameEvent = Game::TIMEOUT_DISCARD;
	static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
	std::uniform_int_distribution<int> distribution(0, 1);
	// Prepare message
	json message;
	message["MessageType"] = "MERCHANT_DISCARD_CARDS_TIMEOUT";
	message["PlayerName"] = m_game->m_playerList[0]->getPlayerName();
	for (auto& card : m_game->m_selectedCards) {
		message["Card"] = Card::m_cardNameToString.at(card->getCardType());
		int randomNum = distribution(generator);
		if (randomNum == 0) {
			message["Pile"] = "LEFT_DISCARD_PILE";
		}
		else if (randomNum == 1) {
			message["Pile"] = "RIGHT_DISCARD_PILE";
		}
		std::string messageString = message.dump();
		Network::getInstance().sendMessage(messageString);
	}
}

void GameLogic::updatePlayerInfo(Player* player, int money, int score, std::unordered_map<Card::CardType, int> playerCardMap, std::unordered_map<Card::CardType, int> blackMarketBonusMap)
{
	// Update player money and score
	player->setPlayerMoney(money);
	player->setPlayerScore(score);

	// Update goods amount
	for (int cardType = Card::APPLE; cardType <= Card::CROSSBOW; cardType++) {
		player->setPlayerGoodsAmount(cardType, playerCardMap[static_cast<Card::CardType>(cardType)]);
	}

	// Update black market status
	for (int cardType = Card::PEPPER; cardType <= Card::SILK; cardType++) {
		if (blackMarketBonusMap[static_cast<Card::CardType>(cardType)] == 1) {
			player->setPlayerMedalStatus(cardType, Player::MedalStatus::BLACK_MARKET);
		}
		else if (blackMarketBonusMap[static_cast<Card::CardType>(cardType)] == 2) {
			player->setPlayerMedalStatus(cardType, Player::MedalStatus::BLACK_MARKET_PLUS);
		}
		else {
			player->setPlayerMedalStatus(cardType, Player::MedalStatus::NONE);
		}
	}
}
