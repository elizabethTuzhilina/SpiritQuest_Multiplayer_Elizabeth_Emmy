#include "TagState.hpp"
#include "Utility.hpp"

TagState::TagState(StateStack& stack, Context context)
	: State(stack, context)
	, m_player(*context.player)
{
	m_top_text.setFont(context.fonts->Get(Font::kMain));
	m_top_text.setCharacterSize(50);
	m_top_text.setFillColor(sf::Color::Black);
	m_top_text.setString("Enter Tag");
	Utility::CentreOrigin(m_top_text);
	m_top_text.setPosition(context.window->getView().getSize().x / 2.f, 200.f);

	m_top_text.setFont(context.fonts->Get(Font::kMain));
	m_top_text.setCharacterSize(50);
	m_top_text.setFillColor(sf::Color::Black);
	m_top_text.setString("Confirm with Enter");
	Utility::CentreOrigin(m_top_text);
	m_top_text.setPosition(context.window->getView().getSize().x / 2.f, context.window->getView().getSize().y - 200.f);

	m_tag_text.setFont(context.fonts->Get(Font::kMain));
	m_tag_text.setCharacterSize(50);
	m_tag_text.setFillColor(sf::Color::Black);
}

void TagState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.draw(m_top_text);
	window.draw(m_bottom_text);
	window.draw(m_tag_text);
}

bool TagState::Update(sf::Time dt)
{
	return false;
}

bool TagState::HandleEvent(const sf::Event& event)
{

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Return)
		{
			RequestStackPop();
		}
		else if (event.key.code == sf::Keyboard::Backspace)
		{
			if (m_tag_ss.gcount() > 0)
			{
				m_tag_ss.clear();
			}
		}
		else
		{
			m_tag_ss << static_cast<char>(event.key.code);
		}
		m_tag_text.setString(m_tag_ss.str());
		Utility::CentreOrigin(m_tag_text);
		m_tag_text.setPosition(GetContext().window->getView().getSize().x / 2.f, GetContext().window->getView().getSize().y / 2.f);
	}

	return false;
}
