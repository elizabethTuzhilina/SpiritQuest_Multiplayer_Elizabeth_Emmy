//E.I D00244320, E.T d00245315
#pragma once
#include "SceneNode.hpp"
#include "CommandQueue.hpp"

class Entity : public SceneNode
{
public:
	Entity(int hitpoints);
	void SetVelocity(sf::Vector2f velocity);
	void SetVelocity(float vx, float vy);
	sf::Vector2f GetVelocity() const;
	void SetHitpoints(int points);
	void Accelerate(sf::Vector2f velocity);
	void Accelerate(float vx, float vy);

	int GetHitPoints() const;
	void Repair(int points);
	void Damage(int points);
	void Deteriorate();
	void Destroy();
	void Remove();
	virtual bool IsDestroyed() const override;

	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	

private:
	sf::Vector2f m_velocity;
	int m_hitpoints;
};

