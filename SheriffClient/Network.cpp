#include "Network.h"

void GameModel::addObserver(std::shared_ptr<Observer> observer)
{
	m_observers.push_back(observer);
}

void GameModel::removeObserver(std::shared_ptr<Observer> observer)
{
	m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

void GameModel::setState(const std::string & newState)
{
	m_state = newState;
	notifyObservers();
}

std::string GameModel::getState() const
{
	return m_state;
}

void GameModel::notifyObservers()
{
	for (const auto& observer : m_observers) {
		observer->onNotify(m_state);
	}
}
