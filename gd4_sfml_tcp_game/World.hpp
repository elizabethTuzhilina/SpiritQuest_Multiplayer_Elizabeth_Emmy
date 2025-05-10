//E.I D00244320, E.T d00245315
#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "SceneNode.hpp"
#include "SceneLayers.hpp"
#include "Character.hpp"
#include "TextureID.hpp"
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"
#include "NetworkNode.hpp"
#include "NetworkProtocol.hpp"
#include "CharacterType.hpp"

#include <array>

class World : private sf::NonCopyable
{
public:
	explicit World
	(sf::RenderTarget& target,
		FontHolder& font,
		SoundPlayer& sounds,
		bool networked = false);
	void Update(sf::Time dt);
	void Draw();

	sf::FloatRect GetViewBounds() const;
	CommandQueue& GetCommandQueue();


	Character* AddCharacter(int identifier);
	void RemoveCharacter(int identifier);
	void SetCurrentBattleFieldPosition(float line_y);
	void SetWorldHeight(float height);

	bool HasAlivePlayer() const;
	bool HasPlayerReachedEnd() const;

	void SetWorldScrollCompensation(float compensation);
	Character* GetCharacter(int identifier) const;
	sf::FloatRect GetBattleFieldBounds() const;
	bool PollGameAction(GameActions::Action& out);

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();
	void DestroyEntitiesOutsideView();
	void HandleCollisions();
	void UpdateSounds();

	
private:
	
	struct SpawnPoint
	{
		CharacterType m_type;
		float m_x;
		float m_y;

		SpawnPoint(CharacterType type, float x, float y)
			: m_type(type), m_x(x), m_y(y)
		{}
	};

private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	float m_scrollspeed;
	float m_scrollspeed_compensation;
	std::vector<Character*> m_player_aircraft;
	CommandQueue m_command_queue;
	std::vector<SpawnPoint> m_enemy_spawn_points;
	BloomEffect m_bloom_effect;
	bool m_networked_world;
	NetworkNode* m_network_node;
	std::unique_ptr<SpriteNode> m_finish_sprite;
	std::vector<SpriteNode*> m_platforms;  // Store platforms for collision checks

};

