#ifndef COLLISION_SHAPES_HPP
#define COLLISION_SHAPES_HPP

#include <SFML/Graphics.hpp>

class Ball : public sf::CircleShape
{
    private:
    public:
        sf::Vector2f acceleration;
        float mass;
        sf::Vector2f position_old;
        sf::Vector2f position_current;
        Ball(float radius, std::size_t pointCount, sf::Vector2f position, float mass, sf::Color fillColor, sf::Color outlineColor);
        Ball (float radius, std::size_t pointCount, sf::Vector2f position, float mass);
        void updatePos(float dt);
        void accelerate(sf::Vector2f acc);
        void setPositionCurrent(sf::Vector2f p);
        ~Ball();
};


#endif // COLLISION_SHAPES_HPP