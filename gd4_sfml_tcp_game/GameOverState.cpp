#include "GameOverState.hpp"
#include "ResourceHolder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Player.hpp"
#include "Utility.hpp"
/*E.T D00245315*/

GameOverState::GameOverState(StateStack& stack, Context context, const std::string& text)
    : State(stack, context)
    , m_game_over_text()
    , m_elapsed_time(sf::Time::Zero)
{

    sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f window_size(context.window->getSize());

    m_game_over_text.setFont(font);

   /* if (context.player->GetMissionStatus() == MissionStatus::kMissionSuccessGhostFL)
    {
        m_game_over_text.setString("The Ghosts have escaped the Reapers! \n \n         The Ghosts Win ");
    }
    */
    if (context.player->GetMissionStatus() == MissionStatus::kMissionSuccessReaperCG)
    {
        m_game_over_text.setString("The Reaper Team has reaped the Ghosts! \n \n           The Reapers Win");
    }
    else
    {
        m_game_over_text.setString("Both Teams have failed...\n \n    Try again");
    }
    
        
    m_game_over_text.setCharacterSize(30);
    Utility::CentreOrigin(m_game_over_text);
    m_game_over_text.setPosition(0.5f * window_size.x, 0.4 * window_size.y);

}

void GameOverState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    //Create a dark semi-transparent background
    sf::RectangleShape background_shape;
    background_shape.setFillColor(sf::Color(100,0 ,100 , 50));
    background_shape.setSize(window.getView().getSize());

    window.draw(background_shape);
    window.draw(m_game_over_text);
}

bool GameOverState::Update(sf::Time dt)
{
    //Show gameover for 3 seconds and then return to the main menu
    m_elapsed_time += dt;
    if (m_elapsed_time > sf::seconds(4))
    {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
    }
    return false;
}

bool GameOverState::HandleEvent(const sf::Event& event)
{
    return false;
}
