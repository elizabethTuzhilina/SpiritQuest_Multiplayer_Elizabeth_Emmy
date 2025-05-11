//E.I D00244320, E.T d00245315
#pragma once
#include "Entity.hpp"
#include "CharacterType.hpp"
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"
#include "ProjectileType.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"
#include "BulletDirection.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"


class Character : public Entity
{
public:
	Character(CharacterType type, const TextureHolder& textures,FontHolder& fonts, sf::FloatRect worldBounds);
	unsigned int GetCategory() const override;

	//void DisablePickups();
	int GetIdentifier();
	void SetIdentifier(int identifier);
	//int GetMissileAmmo() const;

	void IncreaseFireRate();
	void IncreaseFireSpread();
	

	void UpdateTexts();
	void UpdateMovementPattern(sf::Time dt);

	float GetMaxSpeed() const;
	BulletDirection GetBulletDirection() const;
	void FireUp();
	void FireDown();
	void FireLeft();
	void FireRight();
	void CreateBullet(SceneNode& node, const TextureHolder& textures) const;
	void CreateProjectile(SceneNode& node, ProjectileType type, float x_float, float y_offset, const TextureHolder& textures) const;

	bool IsGhost() const;
	bool IsReaper() const;

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	//void Remove() ;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);

	// Check if this character is within the world boundaries
	void CheckWorldBoundaryCollision();
	// Check if this character collides with the platform
	void CheckPlatformCollision(const SpriteNode& platform);


private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);
	
	void CreatePickup(SceneNode& node, const TextureHolder& textures) const;
	void CheckPickupDrop(CommandQueue& commands);
	void UpdateRollAnimation();

	void UpdateBulletDirection();

	


private:
	CharacterType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;

	TextNode* m_spirit_energy_display;
	TextNode* m_missile_display;
	float m_distance_travelled;
	int m_directions_index;

	Command m_fire_command;
	Command m_missile_command;
	Command m_drop_pickup_command;

	unsigned int m_fire_rate;
	unsigned int m_spread_level;
	unsigned int m_missile_ammo;

	bool m_is_firing;

	bool b_is_up;
	bool b_is_down;
	bool b_is_left;
	bool b_is_right;

	BulletDirection b_current_direction;

	bool m_is_launching_missile;
	sf::Time m_fire_countdown;

	bool m_is_marked_for_removal;
	bool m_show_explosion;
	bool m_spawned_pickup;
	bool m_played_explosion_sound;

	int m_identifier;

	
	sf::FloatRect m_world_bounds;

};

