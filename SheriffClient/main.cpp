#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

int main() 
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "My window", sf::Style::Default);

	// Demo draw a card
	sf::Texture texture;
	if (!texture.loadFromFile("C:/Sheriff/Client/SheriffClient/SheriffClient/Images/apple.png"))
	{
		std::cout << "Error loading image";
	}
	texture.setSmooth(true);

	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f(960, 540));
	sprite.setScale(sf::Vector2f(0.5, 0.5));

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// clear the window with black color
		window.clear();

		// draw everything here...
		// window.draw(...);
		

		window.draw(sprite);

		// end the current frame
		window.display();
	}

	return 0;
}
