//E.I D00244320, E.T D00245315
#include "Entity.hpp"
#include <iostream>
#include <chrono>
#include <thread>

Entity::Entity(int hitpoints)
    :m_hitpoints(hitpoints)
{
}

void Entity::SetVelocity(sf::Vector2f velocity)
{
    m_velocity = velocity;
}

void Entity::SetVelocity(float vx, float vy)
{
    m_velocity.x = vx;
    m_velocity.y = vy;
}

sf::Vector2f Entity::GetVelocity() const
{
    return m_velocity;
    
}

void Entity::SetHitpoints(int points)
{
    m_hitpoints = points;
}

void Entity::Accelerate(sf::Vector2f velocity)
{
    m_velocity += velocity;
    
}

void Entity::Accelerate(float vx, float vy)
{
    m_velocity.x += vx;
    m_velocity.y += vy;
    
}

int Entity::GetHitPoints() const
{
    
    return m_hitpoints ;
}

void Entity::Repair(int points)
{
    assert(points > 0);
    //TODO Limit hitpoints
    m_hitpoints += points;
}

void Entity::Damage(int points)
{
    assert(points > 0);
    float less = 5.f;
    m_hitpoints -= points;
    m_velocity.x -= less;
    m_velocity.y -= less;
    
}

void Entity::Destroy()
{
    m_hitpoints = 0;
}

void Entity::Deteriorate()
{
    while (true)
    {
        sf::Time dt;
        float timer = 0.0f;
        const float interval = 2.0f;

        timer = timer * dt.asSeconds();

        if (timer >= interval)
        {

            Damage(5);

            timer = 0.0f;
        }
    }
}

bool Entity::IsDestroyed() const
{
    return m_hitpoints <= 0;
}

void Entity::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
    move(m_velocity * dt.asSeconds());

}

void Entity::Remove()
{
    Destroy();
}
