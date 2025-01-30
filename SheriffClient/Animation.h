#ifndef _ANIMATION__
#define _ANIMATION__

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

class Animation
{
public:
	enum class Type { MOVE, SCALE, MOVE_AND_SCALE, ROTATE, FADE };
	using Callback = std::function<void()>;

	// Move or Move_and_scale animation
	Animation(sf::Transformable& target, Type type, float durationSeconds, sf::Vector2f posEndValue, float scaleEndValue = 1.f, Callback callback = nullptr);
	
	// Scale or Rotate animation
	Animation(sf::Transformable& target, Type type, float endValue, float durationSeconds, Callback callback = nullptr);

	~Animation();

	void start();
	void stop();
	void update(float deltaTime);
	bool isFinished() const;

	//static float linear(float t) { return t; }
	//static float easeIn(float t) { return t * t; }
	//static float easeOut(float t) { return 1 - (1 - t) * (1 - t); }

private:
	sf::Transformable* m_target;
	Type m_type;
	sf::Vector2f m_startPos, m_endPos;
	float m_startScale, m_endScale;
	float m_startRotation, m_endRotation;
	float m_startOpacity, m_endOpacity;
	float m_duration, m_elapsedTime;
	bool m_running;
	Callback m_callback;

};

// Manager class to handle multiple animations
class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();

	void addAnimation(Animation* animation);
	void update(float deltaTime);
	bool isEmpty() const { return animations.empty(); }

private:
	std::vector<Animation*> animations;

};

#endif // !_ANIMATION__




