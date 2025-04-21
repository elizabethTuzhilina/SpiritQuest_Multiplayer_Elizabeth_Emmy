
//E.I D00244320, E.T D00245315
#include "DataTables.hpp"
#include "CharacterType.hpp"
#include "ProjectileType.hpp"
#include "PickupType.hpp"
#include "Character.hpp"
#include "ParticleType.hpp"
#include "Deteriorate.hpp"
#include "ProjectileSpawner.hpp"

//E.I Changed intRect variables
std::vector<CharacterData> InitializeCharacterData()
{
    std::vector<CharacterData> data(static_cast<int>(CharacterType::kCharacterCount));

    data[static_cast<int>(CharacterType::kGhost)].m_hitpoints = 100;
    data[static_cast<int>(CharacterType::kGhost)].m_speed = 200.f;
    data[static_cast<int>(CharacterType::kGhost)].m_fire_interval = sf::seconds(1);
    data[static_cast<int>(CharacterType::kGhost)].m_texture = TextureID::kGhost;
    data[static_cast<int>(CharacterType::kGhost)].m_texture_rect = sf::IntRect(0, 0, 55, 50);
    data[static_cast<int>(CharacterType::kGhost)].m_has_roll_animation = true;

    data[static_cast<int>(CharacterType::kReaper)].m_hitpoints = 100;
    data[static_cast<int>(CharacterType::kReaper)].m_speed = 200.f;
    data[static_cast<int>(CharacterType::kReaper)].m_fire_interval = sf::seconds(1);
    data[static_cast<int>(CharacterType::kReaper)].m_texture = TextureID::kReaper;
    data[static_cast<int>(CharacterType::kReaper)].m_texture_rect = sf::IntRect(0, 0, 55, 50);
    data[static_cast<int>(CharacterType::kReaper)].m_has_roll_animation = false;

    //AI for Raptor
    //data[static_cast<int>(CharacterType::kReaper)].m_directions.emplace_back(Direction(+45.f, 80.f));
    //data[static_cast<int>(CharacterType::kReaper)].m_directions.emplace_back(Direction(-45.f, 160.f));
    //data[static_cast<int>(CharacterType::kReaper)].m_directions.emplace_back(Direction(+45.f, 80.f));


    return data;
}


//E.I Attempted projectile spawner
std::vector<ProjectileSpawnerData> InitializeProjectileSpawnerData()
{
    std::vector<ProjectileSpawnerData> spawnerdata;

    spawnerdata[static_cast<int>(1)].m_fire_interval = sf::seconds(1);
    spawnerdata[static_cast<int>(1)].m_texture = TextureID::kSpawner;
    return spawnerdata;
}

//E.I Changed texture IDs 
std::vector<ProjectileData> InitializeProjectileData()
{
    std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));
    data[static_cast<int>(ProjectileType::kGhostBullet)].m_damage = 10;
    data[static_cast<int>(ProjectileType::kGhostBullet)].m_speed = 300;
    data[static_cast<int>(ProjectileType::kGhostBullet)].m_texture = TextureID::kBullet;
    data[static_cast<int>(ProjectileType::kGhostBullet)].m_texture_rect = sf::IntRect(0, 0, 34, 31);

    data[static_cast<int>(ProjectileType::kReaperBullet)].m_damage = 10;
    data[static_cast<int>(ProjectileType::kReaperBullet)].m_speed = 300;
    data[static_cast<int>(ProjectileType::kReaperBullet)].m_texture = TextureID::kBullet;
    data[static_cast<int>(ProjectileType::kReaperBullet)].m_texture_rect = sf::IntRect(0, 0, 34, 31);


    data[static_cast<int>(ProjectileType::kMissile)].m_damage = 20;
    data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150;
    data[static_cast<int>(ProjectileType::kMissile)].m_texture = TextureID::kMissile;
    data[static_cast<int>(ProjectileType::kMissile)].m_texture_rect = sf::IntRect(0, 0, 15, 32);

    return data;
}

std::vector<PickupData> InitializePickupData()
{
    std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));
    data[static_cast<int>(PickupType::kHealthRefill)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kHealthRefill)].m_texture_rect = sf::IntRect(0, 64, 40, 40);
    data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Character& a)
        {
            a.Repair(25);
        };

    data[static_cast<int>(PickupType::kMissileRefill)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kMissileRefill)].m_texture_rect = sf::IntRect(40, 64, 40, 40);
    
    data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Character::CollectMissile, std::placeholders::_1, 3);

    data[static_cast<int>(PickupType::kFireSpread)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kFireSpread)].m_texture_rect = sf::IntRect(80, 64, 40, 40);
    data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Character::IncreaseFireSpread, std::placeholders::_1);

    data[static_cast<int>(PickupType::kFireRate)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kFireRate)].m_texture_rect = sf::IntRect(120, 64, 40, 40);
    data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Character::IncreaseFireRate, std::placeholders::_1);
    
    return data;
}

std::vector<ParticleData> InitializeParticleData()
{
    std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

    data[static_cast<int>(ParticleType::kPropellant)].m_color = sf::Color(255, 255, 50);
    data[static_cast<int>(ParticleType::kPropellant)].m_lifetime = sf::seconds(0.5f);

    data[static_cast<int>(ParticleType::kSmoke)].m_color = sf::Color(50, 50, 50);
    data[static_cast<int>(ParticleType::kSmoke)].m_lifetime = sf::seconds(2.5f);

    return data;
}


//E.I Attempted Deteriorate Mechanic

/*std::vector<DeteriorateData> InitializeDeteriorateData()
{
    std::vector<DeteriorateData> data;
    
    DeteriorateData deteriorateData;
    deteriorateData.m_damage = 5; 

    
    data.push_back(deteriorateData);

    return data;
}
*/
