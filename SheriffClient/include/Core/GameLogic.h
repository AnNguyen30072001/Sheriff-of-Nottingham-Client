#pragma once
#ifndef _GAME_LOGIC__
#define _GAME_LOGIC__

#include "../include/Core/Game.h"

class GameLogic : public Game
{
public:
	GameLogic(Game* game);
	~GameLogic() = default;

	void updatePlayersMedalStatus();
	void updatePlayerScore(Player* player);

	bool attemptReconnect(float deltaTime);

	void handleDealCardsEvent(const nlohmann::json& jsonMessage);

	void handleStartTurnEvent(std::string playerName);

	void handlePresentEvent();

	void handleGiveBagEvent(const nlohmann::json& jsonMessage);
	void handleOpponentGiveBagEvent(const nlohmann::json& jsonMessage);

	void setupDiscardEvent();

	void handleWithdrawEvent(Game::PileType type);
	void handleDiscardEvent(Game::PileType type, std::string cardName);

	void handleOpponentWithdrawEvent(Game::PileType type, int playerIndex);
	void handleOpponentDiscardEvent(Game::PileType type, int playerIndex, Card::CardType cardType);

	void handleSheriffInspectEvent(const nlohmann::json& jsonMessage);
	void handleSheriffPassEvent(const nlohmann::json& jsonMessage);

	void revealCard(Player* sheriff, std::vector<Card::CardType> cardTypes, int revealIndex, const nlohmann::json& jsonMessage);
	void retrieveCards(const nlohmann::json& jsonMessage);

	void handleTimeoutWithdraw();
	void handleTimeoutDiscard();

private:
	Game* m_game;
	Card::CardType m_goodsReport;
	int	m_bribeAmount;

};

#endif // !_GAME_LOGIC__