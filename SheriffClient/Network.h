#ifndef NETWORK__
#define NETWORK__

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <optional>

#include <SFML/Network.hpp>

// Observer Interface
class Observer {
public:
	virtual void onNotify(const std::string& m_state) = 0;
	virtual ~Observer() = default;
};

// Subject (Game Model)
class GameModel {
public:
	void addObserver(std::shared_ptr<Observer> observer);

	void removeObserver(std::shared_ptr<Observer> observer);

	void setState(const std::string& newState);

	std::string getState() const;

private:
	void notifyObservers();

	std::vector<std::shared_ptr<Observer>> m_observers;
	std::string m_state;
};

#pragma once
class Network
{
public:
	Network(const std::string& serverAddress, unsigned short serverPort);
	~Network();

	bool connect();
	void disconnect();

	bool sendMessage(const std::string& message);
	bool receiveMessage(std::string& outMessage);

	void startListening();
	void stopListening();

	bool isConnected() const;

private:
	sf::TcpSocket m_socket;
	sf::Thread m_listenerThread;
	std::mutex m_messageMutex;
	std::queue<std::string> m_messageQueue;

	std::string m_serverAddress;
	unsigned short m_serverPort;
	bool m_connected;
	bool m_listening;

	void listenToServer();


};

#endif // !NETWORK__



