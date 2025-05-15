
//E.T D00245315
#include "GameState.hpp"
#include "Player.hpp"
#include "MissionStatus.hpp"
#include <iostream>
#include <fstream>

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context),
	m_world(*context.window, *context.fonts, *context.sounds, false),
	m_player(nullptr, 1, context.keys1)  
{
	std::ifstream input_file("ip.txt.txt");
	std::string ip_address, nametag;
	nametag = "EMMY";
	input_file >> ip_address >> nametag;
	context.player = &m_player; 
	m_world.AddCharacter(1, nametag);  // add the character separately
	m_player.SetMissionStatus(MissionStatus::kMissionRunning);
	
	//Play Music
	context.music->Play(MusicThemes::kMissionTheme);
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{
	

	/*
	if (!m_world.HasAlivePlayer())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionSuccessReaperGD);
		RequestStackPush(StateID::kGameOver);
	}
	else if (!m_world.HasAlivePlayer())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionFailure);
		//m_player2.SetMissionStatus(MissionStatus::kMissionFailure);
		RequestStackPush(StateID::kGameOver);
	}
	else if (!m_world.HasAlivePlayer())
	{
		//m_player2.SetMissionStatus(MissionStatus::kMissionSuccessGhostRD);
		RequestStackPush(StateID::kGameOver);
	}
	else if(m_world.HasPlayerReachedEnd())
	{ 
		m_player.SetMissionStatus(MissionStatus::kMissionSuccessGhostFL);
		RequestStackPush(StateID::kGameOver);
	}
	// ET. if the reaper and player reach the end can put in game over state 
	// to be altered for when multiplayer is added to have the reaper win once P1 reaches 0 SE
	else if (m_world.HasPlayerReachedEnd())
	{
		//m_player2.SetMissionStatus(MissionStatus::kMissionFailureReaper);
		RequestStackPush(StateID::kGameOver);
	}
	*/	
	m_world.Update(dt);
	int ghostCount = m_world.CountCharacters(CharacterType::kGhost);
	int reaperCount = m_world.CountCharacters(CharacterType::kReaper);
	bool ghostReachedFinish = m_world.HasPlayerReachedEnd();

	if (ghostCount == 0 && reaperCount > 0 && m_world.GetPlayers()>2)
	{
		GetContext().player->SetMissionStatus(MissionStatus::kMissionSuccessReaperCG);
		RequestStackPush(StateID::kGameOver);
	}
	else if (ghostReachedFinish || reaperCount == 0 && m_world.GetPlayers() > 2)
	{
		GetContext().player->SetMissionStatus(MissionStatus::kMissionSuccessGhostFL);
		RequestStackPush(StateID::kGameOver);
	}

	
	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleRealTimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleEvent(event, commands);

	//Escape should bring up the pause menu
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}
