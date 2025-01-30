#ifndef _TIMER__
#define _TIMER__

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <iostream>

class Timer
{
public:
	Timer(sf::RenderWindow* parentWindow);
	~Timer();

	void start();
	void stop();
	void reset();
	bool isRunning() const;

	void update();
	void render();

private:
	sf::RenderWindow* m_parentWindow;
	sf::Font m_font;					// Font for the timer
	sf::RectangleShape m_timerIcon;		// Timer Icon
	sf::Texture m_timerIconTexture;		// Timer Icon Texture
	sf::Text m_timerText;				// Timer text
	sf::Clock m_clock;					// SFML Clock for timing
	sf::Time m_totalTime;				// Total countdown time
	bool m_isRunning;					// Whether the timer is active

};

#endif // !_TIMER__

