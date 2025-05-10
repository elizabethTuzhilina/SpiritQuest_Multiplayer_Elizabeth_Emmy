//E.I D00244320, E.T d00245315
#pragma once
#include <SFML/Window/Event.hpp>
#include "Action.hpp"
#include "CommandQueue.hpp"
#include "MissionStatus.hpp"

#include "KeyBinding.hpp"
#include <SFML/Network/TcpSocket.hpp>

#include <map>

class Command;


class Player
{
	// ET: adding constructor to handle player IDs 
public:
	//enum class ID{Player1, Player2};
	Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);
	void HandleEvent(const sf::Event& event, CommandQueue& command);
	void HandleRealTimeInput(CommandQueue& command);
	void HandleRealtimeNetworkInput(CommandQueue& commands);

	//React to events or realtime state changes recevied over the network
	void HandleNetworkEvent(Action action, CommandQueue& commands);
	void HandleNetworkRealtimeChange(Action action, bool action_enabled);

	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;

	void DisableAllRealtimeActions();
	bool IsLocal() const;
	bool IsRealTimeAction(Action action);

private:
	void InitialiseActions();

private:
	const KeyBinding* m_key_binding;
	std::map<Action, Command> m_action_binding;
	std::map<Action, bool> m_action_proxies;
	MissionStatus m_current_mission_status;

	int m_identifier;
	sf::TcpSocket* m_socket;
	//ID m_id;//player1 /2 ids
};

