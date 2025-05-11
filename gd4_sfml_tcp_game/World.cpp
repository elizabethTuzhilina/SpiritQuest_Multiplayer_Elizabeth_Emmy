//E.I D00244320, E.T D00245315

#include "World.hpp"
#include "Pickup.hpp"
#include "Projectile.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include "Character.hpp"
#include "SpriteNode.hpp"
#include "SceneNode.hpp"
#include "CharacterType.hpp"
#include "Player.hpp"

#include <iostream>
World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, bool networked)
	: m_target(output_target)
	,m_camera(output_target.getDefaultView())
	,m_textures()
	,m_fonts(font)
	,m_sounds(sounds)
	,m_scenegraph(ReceiverCategories::kNone)
	,m_scene_layers()
	,m_world_bounds(0.f, 0.f, m_camera.getSize().x,3500.f)
	,m_spawn_position(m_camera.getSize().x/2.f, m_world_bounds.height - m_camera.getSize().y/2.f - 100.f)
	//,m_spawn_position2(m_camera.getSize().x / 2.f+2.f, m_world_bounds.height - m_camera.getSize().y / 2.f - 250.f )
	,m_scrollspeed(-50.f)
	, m_scrollspeed_compensation(1.f)
	, m_player_aircraft()
	, m_networked_world(networked)
	, m_network_node(nullptr)
	, m_finish_sprite(nullptr)
	//,m_player_aircraft(nullptr)
	//,m_player2_aircraft(nullptr)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);
	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_spawn_position);
}

void World::SetWorldScrollCompensation(float compensation)
{
	m_scrollspeed_compensation = compensation;
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	m_camera.move(0, m_scrollspeed * dt.asSeconds() * m_scrollspeed_compensation);
	
	for (Character* a : m_player_aircraft)
	{
		a->SetVelocity(0.f, 0.f);
	}
	//m_player2_aircraft->SetVelocity(0.f, 0.f);
	DestroyEntitiesOutsideView();
	

	//Forward commands to the scenegraph
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity();

	HandleCollisions();
	m_scenegraph.RemoveWrecks();
	auto first_to_remove = std::remove_if(m_player_aircraft.begin(), m_player_aircraft.end(), std::mem_fn(&Character::IsMarkedForRemoval));
	m_player_aircraft.erase(first_to_remove, m_player_aircraft.end());
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();

	//ET:
	// Update each player character
	for (Character* player : m_player_aircraft)
	{
		// Check for collisions with each platform
		for (SpriteNode* platform : m_platforms)  // m_platforms is the list of platforms in the world
		{
			if (platform != nullptr && !platform->IsMarkedForRemoval())
			{
				player->CheckPlatformCollision(*platform);
			}  // Check collision with each platform and prevents crash if platform pointer is deleted
		}
		// Check for world boundary collisions
		player->CheckWorldBoundaryCollision();
		
	}
	//std::cout << "Player 1 Velocity: " << m_player_aircraft->GetVelocity().x << ", " << m_player_aircraft->GetVelocity().y << std::endl;
	//std::cout << "Player 2 Velocity: " << m_player2_aircraft->GetVelocity().x << ", " << m_player2_aircraft->GetVelocity().y << std::endl;
}

void World::Draw()
{
	if (PostEffect::IsSupported())
	{
		m_scene_texture.clear();
		m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_scenegraph);
		m_scene_texture.display();
		m_bloom_effect.Apply(m_scene_texture, m_target);
	}
	else
	{
		m_target.setView(m_camera);
		m_target.draw(m_scenegraph);
	}
#ifdef _DEBUG
	for (auto* platform : m_platforms)
	{
		sf::RectangleShape rect;
		sf::FloatRect bounds = platform->GetBoundingRect();
		rect.setPosition(bounds.left, bounds.top);
		rect.setSize({ bounds.width, bounds.height });
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(2.f);
		m_target.draw(rect);
	}
#endif
}

Character* World::GetCharacter(int identifier) const
{
	for (Character* a : m_player_aircraft)
	{
		if (a->GetIdentifier() == identifier)
		{
			return a;
		}
	}
	return nullptr;
}

void World::RemoveCharacter(int identifier)
{
	Character* player = GetCharacter(identifier);
	if (player)
	{
		player->Destroy();
		m_player_aircraft.erase(std::find(m_player_aircraft.begin(), m_player_aircraft.end(), player));
	}
}

Character* World::AddCharacter(int identifier)
{
	int randomint = rand() % 4;


	if (randomint == 1 || randomint == 3)
	{
		std::unique_ptr<Character> player(new Character(CharacterType::kGhost, m_textures, m_fonts, m_world_bounds));
		player->setPosition(5.f, 2200.f);
		player->SetIdentifier(identifier);

		m_player_aircraft.emplace_back(player.get());
		m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(player));
		return m_player_aircraft.back();
	}
	else if (randomint == 0 || randomint == 2)
	{
		std::unique_ptr<Character> player(new Character(CharacterType::kGhost, m_textures, m_fonts, m_world_bounds));
		player->setPosition(0.f, 2200.f);
		player->SetIdentifier(identifier);

		m_player_aircraft.emplace_back(player.get());
		m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(player));
		return m_player_aircraft.back();
		
	}
	
	Character* GetCategory();
}

/*
void World::CreatePickup(sf::Vector2f position, PickupType type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, m_textures));
	pickup->setPosition(position);
	pickup->SetVelocity(0.f, 1.f);
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(pickup));
}
*/

bool World::PollGameAction(GameActions::Action& out)
{
	return m_network_node->PollGameAction(out);
}

void World::SetCurrentBattleFieldPosition(float lineY)
{
	m_camera.setCenter(m_camera.getCenter().x, lineY - m_camera.getSize().y / 2);
	m_spawn_position.y = m_world_bounds.height;
}

void World::SetWorldHeight(float height)
{
	m_world_bounds.height = height;
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

//E.T
bool World::HasAlivePlayer() const
{
	return !m_player_aircraft.empty();
	//return !m_player2_aircraft->IsMarkedForRemoval();
}

//E.T:
bool World::HasPlayerReachedEnd() const
{
	if (Character* player = GetCharacter(1))
	{
		return !m_world_bounds.contains(player->getPosition());
	}
	return false;
	//return !m_world_bounds.contains(m_player2_aircraft->getPosition());
}

//E.I:
void World::LoadTextures()
{
	m_textures.Load(TextureID::kGhost, "Media/Textures/Ghost/ghost-Sheet.png");
	m_textures.Load(TextureID::kReaper, "Media/Textures/Reaper(AnimatedPixelArt)/Preview/Reap.png");
	m_textures.Load(TextureID::kLandscape, "Media/Textures/Clouds 3/1.png");
	m_textures.Load(TextureID::kBullet, "Media/Textures/celestialrock.png");
	m_textures.Load(TextureID::kMissile, "Media/Textures/Missile.png");
	m_textures.Load(TextureID::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(TextureID::kMissileRefill, "Media/Textures/MissileRefill.png");
	m_textures.Load(TextureID::kFireSpread, "Media/Textures/FireSpread.png");
	m_textures.Load(TextureID::kFireRate, "Media/Textures/FireRate.png");
	m_textures.Load(TextureID::kFinishLine, "Media/Textures/FinishLine.png");
	m_textures.Load(TextureID::kBottom, "Media/Textures/Pixel-16px Graveyard/parallax/parallax2(exp).png");
	m_textures.Load(TextureID::kPlat1A, "Media/Textures/Pixel-16px Graveyard/parallax/plat1/plat1-A.png");
	m_textures.Load(TextureID::kPlat1B, "Media/Textures/Pixel-16px Graveyard/parallax/plat1/plat1-B.png");
	m_textures.Load(TextureID::kPlat2A, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/Plat2-A.png");
	m_textures.Load(TextureID::kPlat2B, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/plat2-B.png");
	m_textures.Load(TextureID::kPlat2C, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/plat2-C.png");
	m_textures.Load(TextureID::kPlat2D, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/plat2-D.png");
	m_textures.Load(TextureID::kPlat2E, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/plat2-E.png");
	m_textures.Load(TextureID::kPlat2F, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/plat2-F.png");
	m_textures.Load(TextureID::kPlat2G, "Media/Textures/Pixel-16px Graveyard/parallax/plat2/plat2-G.png");
	m_textures.Load(TextureID::kPlat3, "Media/Textures/Pixel-16px Graveyard/parallax/plat3.png");
	m_textures.Load(TextureID::kEntities, "Media/Textures/Reaper(AnimatedPixelArt)/Preview/Reap(1).png");
	m_textures.Load(TextureID::kJungle, "Media/Textures/Clouds 3/1.png");
	m_textures.Load(TextureID::kCloud, "Media/Textures/Clouds 3/3.png");
	m_textures.Load(TextureID::kExplosion, "Media/Textures/Explosion.png");
	m_textures.Load(TextureID::kParticle, "Media/Textures/Particle.png");
}

//E.T : fix
//E.I:
// 
// Fixed BuildScene() texture_rect duplication and declaration placement
void World::BuildScene()
{
	// Initialize scene layers
	for (int i = 0; i < static_cast<int>(SceneLayers::kLayerCount); ++i)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kLowerAir)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	// Prepare the background
	sf::Texture& texture = m_textures.Get(TextureID::kJungle);
	texture.setRepeated(true);

	float view_height = m_camera.getSize().y;
	sf::IntRect texture_rect(
		static_cast<int>(m_world_bounds.left),
		static_cast<int>(m_world_bounds.top),
		static_cast<int>(m_world_bounds.width),
		static_cast<int>(m_world_bounds.height + view_height)
	);

	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, texture_rect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top - view_height);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(background_sprite));

	// Bottom sprite
	sf::Texture& bottom_texture = m_textures.Get(TextureID::kBottom);
	std::unique_ptr<SpriteNode> bottom_sprite(new SpriteNode(bottom_texture));
	bottom_sprite->setPosition(0.f, 2300.f);
	m_scene_layers[static_cast<int>(SceneLayers::kLowerAir)]->AttachChild(std::move(bottom_sprite));

	// Cloud sprite
	sf::Texture& cloud_texture = m_textures.Get(TextureID::kCloud);
	cloud_texture.setRepeated(true);
	std::unique_ptr<SpriteNode> cloud_sprite(new SpriteNode(cloud_texture));
	cloud_sprite->setPosition(m_world_bounds.left, 2400.f);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(cloud_sprite));

	//cloud_texture.move(40.f, m_scrollspeed);

	//EI: Add Platforms
	//Plat1,A
	sf::Texture& plat1A_texture = m_textures.Get(TextureID::kPlat1A);
	std::unique_ptr<SpriteNode> plat1A_sprite(new SpriteNode(plat1A_texture));
	plat1A_sprite->setPosition(0.f, 2200.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat1A_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat1A_sprite));
	
	//1B
	sf::Texture& plat1B_texture = m_textures.Get(TextureID::kPlat1B);
	std::unique_ptr<SpriteNode> plat1B_sprite(new SpriteNode(plat1B_texture));
	plat1B_sprite->setPosition(600.f, 2200.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat1B_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat1B_sprite));
	
	//Plat2, A
	sf::Texture& plat2a_texture = m_textures.Get(TextureID::kPlat2A);
	std::unique_ptr<SpriteNode> plat2a_sprite(new SpriteNode(plat2a_texture));
	plat2a_sprite->setPosition(300.f, 2000.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2a_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2a_sprite));
	
	//2B
	sf::Texture& plat2b_texture = m_textures.Get(TextureID::kPlat2B);
	std::unique_ptr<SpriteNode> plat2b_sprite(new SpriteNode(plat2b_texture));
	plat2b_sprite->setPosition(80.f, 1800.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2b_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2b_sprite));

	
	//2C
	sf::Texture& plat2c_texture = m_textures.Get(TextureID::kPlat2C);
	std::unique_ptr<SpriteNode> plat2c_sprite(new SpriteNode(plat2c_texture));
	plat2c_sprite->setPosition(700.f, 1750.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2c_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2c_sprite));
	
	//2D
	sf::Texture& plat2d_texture = m_textures.Get(TextureID::kPlat2D);
	std::unique_ptr<SpriteNode> plat2d_sprite(new SpriteNode(plat2d_texture));
	plat2d_sprite->setPosition(0.f, 1600.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2d_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2d_sprite));
	
	//2E
	sf::Texture& plat2e_texture = m_textures.Get(TextureID::kPlat2E);
	std::unique_ptr<SpriteNode> plat2e_sprite(new SpriteNode(plat2e_texture));
	plat2e_sprite->setPosition(800.f, 1550.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2e_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2e_sprite));
	
	//2F
	sf::Texture& plat2f_texture = m_textures.Get(TextureID::kPlat2F);
	std::unique_ptr<SpriteNode> plat2f_sprite(new SpriteNode(plat2f_texture));
	plat2f_sprite->setPosition(150.f, 1380.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2f_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2f_sprite));
	
	//2G
	sf::Texture& plat2g_texture = m_textures.Get(TextureID::kPlat2G);
	std::unique_ptr<SpriteNode> plat2g_sprite(new SpriteNode(plat2g_texture));
	plat2g_sprite->setPosition(0.f, 1150.f);
	//ET:collisions with platfroms
	m_platforms.push_back(plat2g_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2g_sprite));
	

	sf::Texture& plat3_texture = m_textures.Get(TextureID::kPlat3);
	std::unique_ptr<SpriteNode> plat3_sprite(new SpriteNode(plat3_texture));
	plat3_sprite->setPosition(0.f, 0.f);
	//ET:collisions with platfroms
	//m_platforms.push_back(plat3_sprite.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat3_sprite));
	

	//Add the finish line
	sf::Texture& finish_texture = m_textures.Get(TextureID::kFinishLine);
	std::unique_ptr<SpriteNode> finish_sprite(new SpriteNode(finish_texture));
	finish_sprite->setPosition(0.f, -1000.f);
	m_finish_sprite = finish_sprite.get();  
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(finish_sprite));

	// Sound effects
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));
}

void World::AdaptPlayerPosition()
{
	for (Character* aircraft : m_player_aircraft)
	{
		sf::Vector2f position = aircraft->getPosition();

		// Wrap horizontally
		if (position.x < m_world_bounds.left)
			position.x = m_world_bounds.left + m_world_bounds.width;
		else if (position.x > m_world_bounds.left + m_world_bounds.width)
			position.x = m_world_bounds.left;

		// Optional: Wrap vertically if needed
		if (position.y < m_world_bounds.top)
			position.y = m_world_bounds.top + m_world_bounds.height;
		else if (position.y > m_world_bounds.top + m_world_bounds.height)
			position.y = m_world_bounds.top;

		aircraft->setPosition(position);
	}
}

//ET: Player 2 updates 

void World::AdaptPlayerVelocity()
{
	for (Character* player : m_player_aircraft)
	{

		sf::Vector2f velocity = player->GetVelocity();

		//If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			player->SetVelocity(velocity / std::sqrt(2.f));
		}

		//Add scrolling velocity
		//player->Accelerate(0.f, m_scrollspeed);
	}
}


sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize()/2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattleFieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 1600.f;        // extend detection far above
	bounds.height += 1600.f;    // extend detection range

	return bounds;

}

//E.I: Changed kenemyaircraft to Player1.
void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = static_cast<int>(ReceiverCategories::kGhostR) | static_cast<int>(ReceiverCategories::kProjectile) | static_cast<int>(ReceiverCategories::kReaperR);//ET, same constrainst for player 2
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			//Does the object intersect with the battlefield
			//ET: doesnt touch the platforms or background 
			if (!GetBattleFieldBounds().intersects(e.GetBoundingRect()) &&
				dynamic_cast<const SpriteNode*>(&e) == nullptr)
			{
				e.Remove();
			}
		});
	m_command_queue.Push(command);

}



bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{ 
		std::swap(colliders.first, colliders.second);
	}
	else
	{
		return false;
	}
}

bool MatchesCategoriesChar(SceneNode::Pair& colliders, CharacterType type1, CharacterType type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{
		std::swap(colliders.first, colliders.second);
	}
	else
	{
		return false;
	}
}

//E.I: Stopped code that instantly kills enemy (reaper)
void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategoriesChar(pair, CharacterType::kGhost, CharacterType::kReaper))
		{
			auto& ghost = static_cast<Character&>(*pair.first);
			auto& reaper = static_cast<Character&>(*pair.second);
			//Collision response
			ghost.Damage(10);
			//enemy.Destroy();
		}

		//ET: destroy player if player 2 collides with it 
		else if (MatchesCategoriesChar(pair, CharacterType::kGhost, CharacterType::kReaper))
		{
			auto& ghost = static_cast<Character&>(*pair.first);
			auto& reaper = static_cast<Character&>(*pair.second);
			//Collision response
			ghost.Destroy();
			reaper.Damage(5);
		}
		else if (MatchesCategories(pair, ReceiverCategories::kGhostR, ReceiverCategories::kPickup))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Collision response
			pickup.Apply(player);
			pickup.Destroy();
			player.PlayLocalSound(m_command_queue, SoundEffect::kCollectPickup);
		}
		else if (MatchesCategories(pair, ReceiverCategories::kGhostR, ReceiverCategories::kEnemyProjectile) || MatchesCategories(pair, ReceiverCategories::kReaperR, ReceiverCategories::kAlliedProjectile))
		{
			auto& aircraft = static_cast<Character&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Collision response
			aircraft.Damage(projectile.GetDamage());
			projectile.Destroy();
		}
	}
}



void World::UpdateSounds()
{
	// Set listener's position to player position
	sf::Vector2f listener_position;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (m_player_aircraft.empty())
	{
		listener_position = m_camera.getCenter();
	}

	// 1 or more players -> mean position between all aircrafts
	else
	{
		for (Character* player : m_player_aircraft)
		{
			listener_position += player->GetWorldPosition();
		}

		listener_position /= static_cast<float>(m_player_aircraft.size());
	}

	// Set listener's position
	m_sounds.SetListenerPosition(listener_position);

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}
