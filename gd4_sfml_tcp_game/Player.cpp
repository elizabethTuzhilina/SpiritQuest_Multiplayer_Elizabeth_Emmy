//E.I D00244320, E.T D00245315
#include "Player.hpp"
#include "ReceiverCategories.hpp"
#include "CommandQueue.hpp"
#include "Character.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/Network/Packet.hpp>

#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <iostream>

/*ET mostly for multiplayer programming*/
struct CharacterMover
{
    CharacterMover(float vx, float vy, int identifier)
        : velocity(vx, vy)
        , character_id(identifier)
    {
    }

    void operator()(Character& character, sf::Time) const
    {
        if (character.GetIdentifier() == character_id)
        {
            character.Accelerate(velocity * character.GetMaxSpeed());
        }
    }

    sf::Vector2f velocity;
    int character_id;
};

//E.T
struct CharacterFireTrigger
{

  //  const bool isGhost = (id == ID::Player1);

    // PLAYER 1(Ghost) Controls (WASD +Space/M)
   // if (isGhost) {
        //Set initial key bindings
    CharacterFireTrigger(int identifier)
       : character_id(identifier)
    {
    }

    void operator() (Character& character, sf::Time) const
    {
        if (character.GetIdentifier() == character_id)
            character.Fire();
    }

    int character_id;
};

/*struct CharacterMissileTrigger
{
    CharacterMissileTrigger(int identifier)
        : character_id(identifier)
    {
    }

    void operator() (Aircraft& aircraft, sf::Time) const
    {
        if (aircraft.GetIdentifier() == aircraft_id)
            aircraft.LaunchMissile();
    }

    int aircraft_id;
};
*/

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding) : m_key_binding(binding), m_current_mission_status(MissionStatus::kMissionRunning), m_identifier(identifier), m_socket(socket)
{
    //Set initial action bindings
    InitialiseActions();

    //Assign all categories to a player's aircraft
    for (auto& pair : m_action_binding)
    {
        pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
    }

}

void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
    if (event.type == sf::Event::KeyPressed)
    {
        Action action;
        if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && !IsRealtimeAction(action))
        {
            // Network connected -> send event over network
            if (m_socket)
            {
                sf::Packet packet;
                packet << static_cast<sf::Int32>(Client::PacketType::kPlayerEvent);
                packet << m_identifier;
                packet << static_cast<sf::Int32>(action);
                m_socket->send(packet);
            }

            // Network disconnected -> local event
            else
            {
                commands.Push(m_action_binding[action]);
            }
        }
    }


    // Realtime change (network connected)
    if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && m_socket)
    {

        Action action;
        if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && IsRealtimeAction(action))
        {
            
            // Send realtime change over network
            sf::Packet packet;
            packet << static_cast<sf::Int32>(Client::PacketType::kPlayerRealtimeChange);
            packet << m_identifier;
            packet << static_cast<sf::Int32>(action);
            packet << (event.type == sf::Event::KeyPressed);
            m_socket->send(packet);
        }

    }

}



bool Player::IsLocal() const
{
    // No key binding means this player is remote
     return m_key_binding != nullptr;
 }
 
 void Player::DisableAllRealtimeActions()
 {
     for (auto& action : m_action_proxies)
     {
         sf::Packet packet;
         packet << static_cast<sf::Int32>(Client::PacketType::kPlayerRealtimeChange);
         packet << m_identifier;
         packet << static_cast<sf::Int32>(action.first);
         packet << false;
         m_socket->send(packet);
     }
 }

 void Player::HandleRealTimeInput(CommandQueue& commands)
 {
     // Check if this is a networked game and local player or just a single player game
     if ((m_socket && IsLocal()) || !m_socket)
     {
         // Lookup all actions and push corresponding commands to queue
         std::vector<Action> activeActions = m_key_binding->GetRealtimeActions();
         for (Action action : activeActions)
             commands.Push(m_action_binding[action]);
     }
 }



 void Player::HandleRealtimeNetworkInput(CommandQueue& commands)
 {
     if (m_socket && !IsLocal())
     {
         // Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
         for (auto pair : m_action_proxies)
         {
             if (pair.second && IsRealtimeAction(pair.first))
                 commands.Push(m_action_binding[pair.first]);
         }
        
    }
    
}

void Player::HandleNetworkEvent(Action action, CommandQueue& commands)
{
    commands.Push(m_action_binding[action]);
}

void Player::HandleNetworkRealtimeChange(Action action, bool actionEnabled)
{
    m_action_proxies[action] = actionEnabled;
}

void Player::SetMissionStatus(MissionStatus status)
{
    m_current_mission_status = status;
}

MissionStatus Player::GetMissionStatus() const
{
    return m_current_mission_status;
}



void Player::InitialiseActions()
{
    m_action_binding[Action::kMoveLeft].action = DerivedAction<Character>(CharacterMover(-1, 0.f, m_identifier));
    m_action_binding[Action::kMoveRight].action = DerivedAction<Character>(CharacterMover(+1, 0.f, m_identifier));
    m_action_binding[Action::kMoveUp].action = DerivedAction<Character>(CharacterMover(0.f, -1, m_identifier));
    m_action_binding[Action::kMoveDown].action = DerivedAction<Character>(CharacterMover(0.f, 1, m_identifier));
    m_action_binding[Action::kBulletFire].action = DerivedAction<Character>(CharacterFireTrigger(m_identifier));
   // m_action_binding[Action::kMissileFire].action = DerivedAction<Character>(CharacterMissileTrigger(m_identifier));
}
