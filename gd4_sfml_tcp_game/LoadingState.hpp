#pragma once

#include "State.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "Label.hpp"
#include "ResourceIdentifiers.hpp"

class LoadingState : public State
{
public:
	LoadingState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite m_background_sprite;
	gui::Container m_gui_container_one;

	std::vector<gui::Button::Ptr> m_buttons_one;

	bool m_is_player_one_ready;

	sf::Sprite m_sprite_one;

	TextureHolder m_textures;
};