#pragma once

#include "State.hpp"
#include <SFML/Graphics/Text.hpp>
#include <sstream>

class TagState : public State
{
public:
	TagState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	Player& m_player;
	sf::Text m_top_text;
	sf::Text m_bottom_text;
	sf::Text m_tag_text;

	std::stringstream m_tag_ss;
};