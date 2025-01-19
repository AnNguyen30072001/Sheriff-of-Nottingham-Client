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

Network::Network(const std::string & serverAddress, unsigned short serverPort) : 
	m_serverAddress(serverAddress), 
	m_serverPort(serverPort), 
	m_connected(false), 
	m_listening(false),
	m_listenerThread(&Network::listenToServer, this)
{

}

Network::~Network()
{
	disconnect();
}

bool Network::connect()
{
	if (m_socket.connect(m_serverAddress, m_serverPort) == sf::Socket::Done) {
		m_connected = true;
		return true;
	}
	return false;
}

void Network::disconnect()
{
	if (m_connected) {
		m_socket.disconnect();
		m_connected = false;
		stopListening();
	}
}

bool Network::sendMessage(const std::string & message)
{
	if (!m_connected) return false;

	sf::Packet packet;
	packet << message;
	return (m_socket.send(packet) == sf::Socket::Done);
}

bool Network::receiveMessage(std::string& outMessage)
{
	// Lock the mutex to ensure thread-safe access to the message queue
	std::lock_guard<std::mutex> lock(m_messageMutex);

	// Check if the message queue has any messages
	if (!m_messageQueue.empty()) {
		outMessage = m_messageQueue.front(); // Retrieve the front message
		m_messageQueue.pop();               // Remove the message from the queue
		return true;                      // Indicate a message was retrieved
	}

	return false; // No messages in the queue
}

void Network::startListening()
{
	if (!m_listening) {
		m_listening = true;
		m_listenerThread.launch();
	}
}

void Network::stopListening()
{
	m_listening = false; // Signal the thread to stop
	m_listenerThread.wait(); // Wait for the thread to finish execution
}

bool Network::isConnected() const
{
	return m_connected;
}

void Network::listenToServer()
{
	while (m_listening) {
		char buffer[1024];
		std::size_t received;
		sf::Socket::Status status = m_socket.receive(buffer, sizeof(buffer), received);

		if (status == sf::Socket::Done) {
			std::string message(buffer, received); // Convert buffer to string
			std::lock_guard<std::mutex> lock(m_messageMutex);
			m_messageQueue.push(message); // Add the message to the queue
		}
		else if (status == sf::Socket::Disconnected) {
			std::cerr << "Server disconnected." << std::endl;
			m_listening = false;
			m_connected = false;
		}
		else if (status != sf::Socket::NotReady) {
			std::cerr << "Error receiving data from server." << std::endl;
		}
	}
}
