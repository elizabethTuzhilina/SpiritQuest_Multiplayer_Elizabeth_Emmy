//E.I D00244320, E.T d00245315
#pragma once
#include "SceneNode.hpp"
class SpriteNode : public SceneNode
{
public:
	explicit SpriteNode(const sf::Texture& texture);
	SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect);
	sf::FloatRect GetBoundingRect() const;
private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Sprite m_sprite;
};

