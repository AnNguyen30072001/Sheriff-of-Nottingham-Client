#include "Animation.h"
#include <iostream>

Animation::Animation(sf::Transformable& target, Type type, float durationSeconds, sf::Vector2f posEndValue, float scaleEndValue, float delay, Callback callback)
	: m_target(&target), m_type(type), m_endPos(posEndValue), m_duration(durationSeconds), m_delay(delay), m_callback(callback), m_elapsedTime(0), 
	m_running(false), m_endDelay(false) {
	if (type == Type::MOVE) {
		m_startPos = target.getPosition();
	}
	else if (type == Type::MOVE_AND_SCALE) {
		m_startPos = target.getPosition();
		m_startScale = target.getScale().x;
		m_endScale = scaleEndValue;
	}
}

Animation::Animation(sf::Transformable& target, Type type, float endValue, float duration, float delay, Callback callback)
	: m_target(&target), m_type(type), m_duration(duration), m_delay(delay), m_callback(callback), m_elapsedTime(0), 
	m_running(false), m_endDelay(false) {
	if (type == Type::SCALE) {
		m_startScale = target.getScale().x;
		m_endScale = endValue;
	}
	else if (type == Type::ROTATE) {
		m_startRotation = target.getRotation();
		m_endRotation = endValue;
	}
}

Animation::~Animation()
{

}

void Animation::start() {
	m_running = true;
	m_elapsedTime = 0;
}

void Animation::stop() {
	m_running = false;
}

void Animation::update(float deltaTime) {
	if (!m_running) return;

	// No animation before delay ends
	if (m_elapsedTime < m_delay) {
		m_elapsedTime += deltaTime;
		return;
	}
	if (!m_endDelay) {
		m_endDelay = true;
		m_elapsedTime -= m_delay;
	}
	m_elapsedTime += deltaTime;
	float progress = m_elapsedTime / m_duration;

	if (m_type == Type::MOVE) {
		sf::Vector2f newPos = m_startPos + (m_endPos - m_startPos) * progress;
		//std::cout << "Newpos: " << newPos.x << ", " << newPos.y << std::endl;
		m_target->setPosition(newPos);
	}
	else if (m_type == Type::SCALE) {
		float newScale = m_startScale + (m_endScale - m_startScale) * progress;
		m_target->setScale(newScale, newScale);
	}
	else if (m_type == Type::MOVE_AND_SCALE) {
		sf::Vector2f newPos = m_startPos + (m_endPos - m_startPos) * progress;
		float newScale = m_startScale + (m_endScale - m_startScale) * progress;
		m_target->setPosition(newPos);
		m_target->setScale(newScale, newScale);
	}
	else if (m_type == Type::ROTATE) {
		float newRotation = m_startRotation + (m_endRotation - m_startRotation) * progress;
		m_target->setRotation(newRotation);
	}

	// Ensure animation stops correctly
	if (m_elapsedTime >= m_duration) {
		stop();

		// Explicitly set the final values
		if (m_type == Type::MOVE) {
			m_target->setPosition(m_endPos);
		}
		else if (m_type == Type::SCALE) {
			m_target->setScale(m_endScale, m_endScale);
		}
		else if (m_type == Type::MOVE_AND_SCALE) {
			m_target->setPosition(m_endPos);
			m_target->setScale(m_endScale, m_endScale);
		}
		else if (m_type == Type::ROTATE) {
			m_target->setRotation(m_endRotation);
		}

		if (m_callback) {
			m_callback(); // Execute callback when animation completes
		}
	}
}

bool Animation::isFinished() const
{
	return (!m_running);
}

// Animation Manager Implementation
void AnimationManager::addAnimation(Animation* animation) {
	animations.push_back(animation);
	animation->start();
}

void AnimationManager::cancel()
{
	for (int i = 0; i < animations.size(); i++) {
		animations[i]->stop();
		delete animations[i];
		animations.erase(animations.begin() + i);
	}
}

AnimationManager::AnimationManager()
{
}

AnimationManager::~AnimationManager()
{
	for (auto& animation : animations) {
		delete animation;
	}
}

void AnimationManager::update(float deltaTime) {
	for (int i = 0; i < animations.size(); i++) {
		animations[i]->update(deltaTime);
		if (animations[i]->isFinished()) {
			delete animations[i];
			animations.erase(animations.begin() + i);
		}
	}
}