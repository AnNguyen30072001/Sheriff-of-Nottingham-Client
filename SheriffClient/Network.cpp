#include "Network.h"

Network::Network(const std::string & serverAddress, unsigned short serverPort) : 
	m_serverAddress(serverAddress), 
	m_serverPort(serverPort), 
	m_listening(false),
	m_connected(false),
	m_listenerThread(&Network::listenToServer, this),
	m_processing(true)
{

}

Network::~Network()
{
	disconnect();
}

void Network::addObserver(Observer * observer)
{
	if (std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
		m_observers.push_back(observer);
	}
}

void Network::removeObserver(Observer * observer)
{
	m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

void Network::startProcessingMessageQueue()
{
	m_processing = true;
	std::thread(&Network::processMessages, this).detach();  // Run consumer loop in a separate thread
}

void Network::stopProcessingMessageQueue()
{
	{
		std::lock_guard<std::mutex> lock(m_messageMutex);
		m_processing = false;
	}
	m_messageCondition.notify_all();  // Wake up the consumer thread
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

bool Network::send(const std::string & message)
{
	if (!m_connected) return false;

	sf::Packet packet;
	packet << message;
	return (m_socket.send(packet) == sf::Socket::Done);
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
			m_messageQueue.push(message);
			m_messageCondition.notify_one();  // Wake up the consumer thread
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

void Network::notifyObservers(const std::string & message)
{
	for (Observer* observer : m_observers) {
		if (observer) {
			observer->onMessageReceived(message);
		}
	}
}

void Network::processMessages()
{
	while (m_processing) {
		std::unique_lock<std::mutex> lock(m_messageMutex);

		// Wait until there are messages in the queue or processing stops
		m_messageCondition.wait(lock, [this] {
			return !m_messageQueue.empty() || !m_processing;
		});

		// Process all messages in the queue
		while (!m_messageQueue.empty()) {
			std::string message = m_messageQueue.front();
			m_messageQueue.pop();

			lock.unlock();  // Release the lock while notifying observers
			notifyObservers(message);
			lock.lock();    // Reacquire the lock for the next iteration
		}
	}
}
