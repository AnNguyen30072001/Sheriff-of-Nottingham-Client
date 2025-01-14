#ifndef COLORMAP__
#define COLORMAP__

#include <SFML/Graphics.hpp>

struct ColorHash {
	std::size_t operator()(const sf::Color& color) const {
		// Combine RGBA components into a single hashable value
		return (static_cast<std::size_t>(color.r) << 24) |
			(static_cast<std::size_t>(color.g) << 16) |
			(static_cast<std::size_t>(color.b) << 8) |
			static_cast<std::size_t>(color.a);
	}
};

struct ColorEqual {
	bool operator()(const sf::Color& c1, const sf::Color& c2) const {
		return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
	}
};

#endif // !COLORMAP__




