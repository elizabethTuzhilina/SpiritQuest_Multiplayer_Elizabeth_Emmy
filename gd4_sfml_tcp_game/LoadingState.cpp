#include "LoadingState.hpp"
#include "Command.hpp"
#include "ReceiverCategories.hpp"
#include "Character.hpp"

#include "Player.hpp"

//Dominik Hampejs D00250604
LoadingState::LoadingState(StateStack& stack, Context context) :
	State(stack, context),
	
	m_is_player_one_ready(false),

	m_textures()
{
	sf::Texture& texture = context.textures->Get(TextureID::kTitleScreen);
	m_background_sprite.setTexture(texture);
	m_background_sprite.setScale(2.f, 2.38f);




	//Set up the ready buttons for the players to confirm their colours
	auto ready_button_one = std::make_shared<gui::Button>(context);
	ready_button_one->setPosition(175, 650);
	ready_button_one->SetText("Confirm");
	ready_button_one->SetCallback([this]()
		{
			std::string text;
			if (m_is_player_one_ready)
			{
				m_is_player_one_ready = false;
				text = "Confirm";
			}
			else {
				text = "Ready";
				m_is_player_one_ready = true;
			}
			m_buttons_one[3]->SetText(text);
		});


	m_buttons_one.push_back(ready_button_one);


	//Place the buttons in the container for player one
	for (auto& button : m_buttons_one) {
		m_gui_container_one.Pack(button);
	}


	//Set up the labels for the players
	auto player_one_label = std::make_shared<gui::Label>("Player One", *context.fonts);
	player_one_label->setPosition(224, 305);
	

	auto player_two_label = std::make_shared<gui::Label>("Player Two", *context.fonts);
	player_two_label->setPosition(674, 305);
	

	m_gui_container_one.Pack(player_one_label);

	//Set up the title label
	auto title_label = std::make_shared<gui::Label>("Loading...", *context.fonts);
	title_label->setPosition(310, 100);
	

	m_gui_container_one.Pack(title_label);

}
//Dominik Hampejs D00250604
void LoadingState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());
	//window.draw(m_background_sprite);
	window.draw(m_gui_container_one);
	window.draw(m_sprite_one);

}
//Dominik Hampejs D00250604
bool LoadingState::Update(sf::Time dt)
{
	//If both players are ready, set the colours and move to the game state
	if (m_is_player_one_ready) {

		RequestStackPop();
	}
	return false;
}

//Dominik Hampejs D00250604
bool LoadingState::HandleEvent(const sf::Event& event)
{
	bool is_one_colour_selecting = false;

	//If the players are not ready, allow them to select their colours
	if (!(m_is_player_one_ready))
	{
		//If the escape key is pressed, go to the pause state
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			RequestStackPush(StateID::kPause);
		}

		//Check if the players are selecting their colours
		for (int i = 0; i < 3; i++)
		{
			is_one_colour_selecting = m_buttons_one[i]->IsActive();

			//If the player one is selecting their colour, allow them to change it
			if (is_one_colour_selecting)
			{
				if (event.type == sf::Event::KeyPressed) {
					//Pressing W or S will deactivate the button
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S)
					{
						m_buttons_one[i]->Deactivate();
					}
					int add = 0;
					//Pressing D or A will change the colour
					if (event.key.code == sf::Keyboard::D)
					{
						add = 20;
					}
					else if (event.key.code == sf::Keyboard::A)
					{
						add = -20;
					}
					//Change the colour based on the button pressed
					
				}
				
			}
		}

		m_gui_container_one.HandleEvent(event);
	}
	return false;
}