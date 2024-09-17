/*
* Source file for CollisionShapes.hpp
* Stores the classes for shapes used in 2d collision
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include "../../include/CollisionShapes.hpp"

/*
                            * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
                            * * * * * * * * * * * * C O N T R U C T O R S * * * * * * * * * * * *
                            * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*/
Ball::Ball(float radius, std::size_t pointCount, sf::Vector2f position, float mass, sf::Color fillColor, sf::Color outlineColor)
{
    this->setRadius(radius);
    this->setOrigin(radius, radius); // Set the origin to the center of the circle
    this->setPosition(position);
    this->setPointCount(pointCount);
    this->setFillColor(fillColor);
    this->setOutlineColor(outlineColor);
    this->mass = mass;
    this->acceleration = sf::Vector2f(0, 0);
    this->position_old = position;
    this->position_current = position;
}
Ball::Ball(float radius=1, std::size_t pointCount = 30, sf::Vector2f position = sf::Vector2f(0, 0), float mass = 1)
{
    this->setRadius(radius);
    this->setOrigin(radius, radius); // Set the origin to the center of the circle
    this->setPosition(position);
    this->setPointCount(pointCount);
    this->mass = mass;
    this->acceleration = sf::Vector2f(0, 0);
    this->position_old = position;
    this->position_current = position;
}

Ball::~Ball()
{
    // Destructor
    delete this;
}

void Ball::updatePos(float dt)
{
    // Update via the Verlet integration method     
    const sf::Vector2f velocity = this->position_current - this->position_old; // Calculate the velocity 

    this->position_old = this->position_current; // Store the current position

    // Update the position
    this->position_current = this->position_current + velocity + this->acceleration * dt * dt;
    this->setPosition(this->position_current);

    // Reset acceleration
    this->acceleration = sf::Vector2f(0, 0);
}

void Ball::accelerate(sf::Vector2f acc)
{
    this->acceleration += acc;
}

void Ball::setPositionCurrent(sf::Vector2f p)
{
    this->position_current = p;
}




