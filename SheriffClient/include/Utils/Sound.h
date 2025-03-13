#ifndef _SOUND__
#define _SOUND__

#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>

class Sound {
public:
	enum class Type { EFFECT, MUSIC };

	Sound();
	~Sound();

	void play(const std::string& name, Type type = Type::EFFECT, float playingOffset = 0.f, float volume = 100.f, float pitch = 1.f,
		std::function<void()> callback = nullptr);

	void stop();

private:
	sf::SoundBuffer m_buffer; // Stores sound effect buffers
	sf::Sound m_activeSound; // Active sound effects
	sf::Music m_activeMusic; // Active music
	std::mutex m_mutex;
};

#endif // !_SOUND__

