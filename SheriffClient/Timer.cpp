#include "Timer.h"

Timer::Timer(sf::RenderWindow* parentWindow) : m_parentWindow(parentWindow)
{
	if (!m_font.loadFromFile("arial-font/arial.ttf")) {
		std::cerr << "Error loading timer font!\n";
	}
	if (!m_timerIconTexture.loadFromFile("Images/TimerIcon.png")) {
		std::cerr << "Error loading timer icon!\n";
	}

	// Set total time to 1 minute
	m_totalTime = sf::seconds(60.f);
	m_isRunning = false;

	// Timer icon setup
	m_timerIcon.setSize(sf::Vector2f(100.f, 100.f));
	m_timerIcon.setFillColor(sf::Color::White);
	m_timerIcon.setTexture(&m_timerIconTexture);
	m_timerIcon.setOutlineThickness(2.f);
	m_timerIcon.setOutlineColor(sf::Color::Black);

	// Timer text setup
	m_timerText.setFont(m_font);
	m_timerText.setCharacterSize(60);
	m_timerText.setFillColor(sf::Color::White);
	m_timerText.setString("01:00");

	// Position the timer at the bottom right of the window
	m_timerIcon.setPosition(sf::Vector2f(1585.f, 890.f));
	m_timerText.setPosition(sf::Vector2f(1700.f, 905.f));
	//float textWidth = m_timerText.getLocalBounds().width;
	//float textHeight = m_timerText.getLocalBounds().height;
	//m_timerIcon.setOrigin(m_timerIcon.getSize().x / 2, m_timerIcon.getSize().y / 2);
	//m_timerText.setOrigin(textWidth / 2, textHeight / 2);
}

Timer::~Timer()
{
}

void Timer::start()
{
	m_isRunning = true;
	m_clock.restart(); // Restart clock when starting timer
}

void Timer::stop()
{
	m_isRunning = false;
}

void Timer::reset()
{
	m_totalTime = sf::seconds(60.f);
	m_isRunning = false;
	m_timerText.setString("01:00");
}

bool Timer::isRunning() const
{
	return m_isRunning;
}

void Timer::update()
{
	if (!m_isRunning) return;

	sf::Time elapsed = m_clock.getElapsedTime();
	sf::Time remainingTime = m_totalTime - elapsed;

	if (remainingTime.asSeconds() <= 0) {
		m_isRunning = false;
		m_timerText.setString("00:00");
	}
	else {
		int seconds = static_cast<int>(remainingTime.asSeconds());
		int minutes = seconds / 60;
		seconds %= 60;

		std::string timeStr = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
			(seconds < 10 ? "0" : "") + std::to_string(seconds);
		m_timerText.setString(timeStr);
	}
}

void Timer::render()
{
	m_parentWindow->draw(m_timerIcon);
	m_parentWindow->draw(m_timerText);
}
