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
#include "json.hpp"
using json = nlohmann::json;

// Observer Interface
class Observer {
public:
	virtual void onMessageReceived(const nlohmann::json& jsonMessage) = 0;
	virtual ~Observer() = default;
};

class Network
{
public:
	// Server: 34.92.181.81
	static Network& getInstance(const std::string& address = "34.92.181.81", uint16_t port = 8080)
	{
		static Network instance(address, port);
		return instance;
	}

	// Delete copy constructor and assignment operator
	Network(const Network&) = delete;
	Network& operator=(const Network&) = delete;

	void addObserver(Observer* observer);
	void removeObserver(Observer* observer);

	void startProcessingMessageQueue();
	void stopProcessingMessageQueue();

	bool connect();
	void disconnect();

	static bool sendMessage(const std::string& msg)
	{
		std::cout << "Send: " << msg << std::endl;
		return getInstance().send(msg);
	}

	bool respondMessage(const nlohmann::json& jsonMessage);

	bool send(const std::string& message);

	void startListening();
	void stopListening();

	bool isConnected() const;

	void setUserPlayerName(const std::string name);
	std::string getUserPlayerName() const;

private:
	// Private constructor and destructor for Singleton
	Network(const std::string& address, uint16_t port);
	~Network();

	std::vector<Observer*> m_observers;
	sf::TcpSocket m_socket;
	sf::Thread m_listenerThread;
	std::mutex m_messageMutex;
	std::queue<std::string> m_messageQueue;
	std::condition_variable m_messageCondition;

	std::string m_serverAddress;
	unsigned short m_serverPort;
	bool m_connected;
	bool m_listening;
	bool m_processing;

	std::string m_userPlayerName;

	void listenToServer();

	void notifyObservers(const std::string& message);
	void processMessages();  // Consumer loop for processing the queue
};


#endif // !NETWORK__



