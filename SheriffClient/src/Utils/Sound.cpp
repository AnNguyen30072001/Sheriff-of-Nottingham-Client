#include "../Utils/Sound.h"
#include <iostream>

Sound::Sound() {}

Sound::~Sound() {
	stop();
}

void Sound::play(const std::string& name, Type type, float playingOffset, float volume, float pitch, std::function<void()> callback) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (type == Type::EFFECT) {
		// Load sound buffer if not already loaded
		if (!m_buffer.loadFromFile("assets/Sound/" + name)) {
			std::cerr << "Failed to load sound: " << name << std::endl;
			return;
		}

		// Set properties
		m_activeSound.setBuffer(m_buffer);
		m_activeSound.setVolume(volume);
		m_activeSound.setPitch(pitch);

		// Play sound
		m_activeSound.play();
		m_activeSound.setPlayingOffset(sf::seconds(playingOffset));
	}

	else if (type == Type::MUSIC) {
		// Stop existing music with the same name
		stop();

		// Load music
		auto music = std::make_unique<sf::Music>();
		if (!music->openFromFile("assets/Sound/" + name)) {
			std::cerr << "Failed to load music: " << name << std::endl;
			return;
		}

		// Set properties
		m_activeMusic.setLoop(true);
		m_activeMusic.setVolume(volume);
		m_activeMusic.setPitch(pitch);
		m_activeMusic.play();
	}
}

void Sound::stop() {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_activeSound.stop();

	m_activeMusic.stop();
}

//void Sound::update() {
//	std::lock_guard<std::mutex> lock(m_mutex);
//
//	// Remove finished sound effects
//	m_activeSound.erase(
//		std::remove_if(m_activeSound.begin(), m_activeSound.end(),
//			[](SoundInstance& instance) {
//		return instance.sound.getStatus() == sf::Sound::Stopped;
//	}),
//		m_activeSound.end());
//
//	// Remove finished music
//	m_activeMusic.erase(
//		std::remove_if(m_activeMusic.begin(), m_activeMusic.end(),
//			[](MusicInstance& instance) {
//		return instance.music && instance.music->getStatus() == sf::Music::Stopped;
//	}),
//		m_activeMusic.end());
//}