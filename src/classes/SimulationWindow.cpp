#include <SFML/Graphics.hpp>
#include <memory>
#include <cmath>
#include "../../include/SimulationWindow.hpp"
#include "../../include/CollisionShapes.hpp"
#include <iostream>

/////////PRIVATE_FUNCTIONS/////////
void windowSimulation::applyGravity()
{
    for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++) {
        auto shape = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);
        shape->accelerate(gravity);
    }
}

void windowSimulation::applyCircularConstraint()
{
    const sf::Vector2f center = sf::Vector2f(this->getSize().x / 2.0f, this->getSize().y / 2.0f);
    const float radius = this->getSize().y / 2.0f;
    // All calculations are about the center of the object
    for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++) {
        auto shape = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);
        sf::Vector2f position = shape->getPosition();
        sf::Vector2f direction = position - center;

        // Distance from center
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > radius - shape->getRadius()) {
            //std::cout << "Position-1:" << shape->position_old.x << ", " << shape->position_old.y << std::endl;
            //std::cout << "Position0: " << shape->position_current.x << ", " << shape->position_current.y << std::endl;

            sf::Vector2f v = shape->position_current - shape->position_old;

            sf::Vector2f n = direction / distance;
            shape->setPositionCurrent(center + n * (radius - shape->getRadius()) * 1.f);

            //std::cout << "Position1: " << shape->position_current.x << ", " << shape->position_current.y << std::endl;

            // Bounce

            //////////////////TODO: FIX BOUNCE - when energy is "conserved"//////////////////////
            sf::Vector2f v_normal = n * (v.x * n.x + v.y * n.y);
            sf::Vector2f v_tangent = v - v_normal;
            sf::Vector2f v_reflected = (v_tangent - v_normal) * .99f;

            shape->position_old = shape->position_current - v_reflected;

            //std::cout << "Position2: " << shape->position_old.x << ", " << shape->position_old.y << std::endl;
        }
    }
}

void windowSimulation::applyCollision()
{
    const sf::Uint32 numObjects = nonStaticObjects.size();

    for (sf::Uint32 i = 0; i < numObjects; i++) {
        auto shape1 = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);

        for (sf::Uint32 j = i + 1; j < numObjects; j++) {
            auto shape2 = std::dynamic_pointer_cast<Ball>(nonStaticObjects[j]);
            const sf::Vector2f direction = shape1->position_current - shape2->position_current;
            const float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
            if (distance < shape1->getRadius() + shape2->getRadius()) {

                const sf::Vector2f normal = direction / distance;
                const float delta = shape1->getRadius() + shape2->getRadius() - distance;

                shape1->position_current += normal * delta / 2.0f;
                shape2->position_current -= normal * delta / 2.0f;

                /*
                // Collision
                sf::Vector2f v1 = shape1->position_current - shape1->position_old;
                sf::Vector2f v2 = shape2->position_current - shape2->position_old;
                sf::Vector2f v1_normal = v1;
                sf::Vector2f v2_normal = v2;
                sf::Vector2f v1_tangent = sf::Vector2f(0, 0);
                sf::Vector2f v2_tangent = sf::Vector2f(0, 0);

                // Normal
                sf::Vector2f direction = shape1->position_current - shape2->position_current;
                float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
                direction = direction / distance;
                v1_normal = direction * (v1.x * direction.x + v1.y * direction.y);
                v2_normal = direction * (v2.x * direction.x + v2.y * direction.y);

                // Tangent
                v1_tangent = v1 - v1_normal;
                v2_tangent = v2 - v2_normal;

                // Bounce
                sf::Vector2f v1_reflected = v1_tangent - v1_normal * 0.9f;
                sf::Vector2f v2_reflected = v2_tangent - v2_normal * 0.9f;

                shape1->position_old = shape1->position_current - v1_reflected;
                shape2->position_old = shape2->position_current - v2_reflected;
                */
            }
        }
    }
}

/////////CONSTRUCTOR/////////
windowSimulation::windowSimulation(const sf::VideoMode &videoMode, const sf::String &title, sf::Uint32 style, 
                const sf::ContextSettings &settings, const float posRatioX, const float posRatioY, 
                bool resizable, bool isFocused, int frameRate, std::shared_ptr<sf::Shape> constraint = nullptr, sf::Color backgroundColor = sf::Color::Black) 
                : sf::RenderWindow(videoMode, title, style, settings) // Inherit from sf::RenderWindow
{
    this->setFramerateLimit(60);
    this->setActive(true);

    if (isFocused)
        this->requestFocus();
    this->isFocused = isFocused;

    //this->setVerticalSyncEnabled(resizable);

    // Constraint for the objects
    if (constraint != nullptr) {
        this->ConstraintShape = constraint;
    }
    else { // Set constraint to be window size
        sf::RectangleShape rectangularArea(sf::Vector2f(this->getSize().x, this->getSize().y));
        rectangularArea.setFillColor(sf::Color::White);
        rectangularArea.setOrigin(this->getSize().x / 2.0f, this->getSize().y / 2.0f);
        //shapes.push_back(std::make_shared<sf::RectangleShape>(rectangularArea));
        this->ConstraintShape = std::make_shared<sf::RectangleShape>(rectangularArea);
    }

    // Set the window position
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int monitorWidth = desktop.width;
    int monitorHeight = desktop.height;
    this->setPosition(sf::Vector2i(monitorWidth * posRatioX, monitorHeight * posRatioY));

    this->backgroundColor = backgroundColor;
}

/////////DESTRUCTOR/////////
windowSimulation::~windowSimulation() 
{
    //delete this;
}

/////////FUNCTIONS/////////
void windowSimulation::draw(const sf::Drawable &drawable, const sf::RenderStates &states = sf::RenderStates::Default)
{
    this->sf::RenderWindow::draw(drawable, states);
}

void windowSimulation::update_visuals()
{
    this->clear(backgroundColor);
    draw(*ConstraintShape);
    for (int i = 0; i < static_cast<int>(shapes.size()); i++) {
        this->draw(*shapes[i]);
    }
    for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++) {
        this->draw(*nonStaticObjects[i]);
    }
    this->display();
}

void windowSimulation::update_positions(float dt)
{
    applyGravity();
    applyCircularConstraint();
    applyCollision();
    for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++) {
        auto shape = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);
        shape->updatePos(dt);
        // std::cout << "Position: " << shape->getPosition().x << ", " << shape->getPosition().y << std::endl;
    }
}

void windowSimulation::add_shape(std::shared_ptr<sf::Shape> shape)
{
    shapes.push_back(shape);
}

void windowSimulation::add_non_static_object(std::shared_ptr<sf::Shape> shape)
{
    nonStaticObjects.push_back(shape);
}

void windowSimulation::remove_shape(std::shared_ptr<sf::Shape> shape)
{
    for (int i = 0; i < static_cast<int>(shapes.size()); i++) {
        if (shapes[i] == shape) {
            shapes.erase(shapes.begin() + i);
            break;
        }
    }
}

void windowSimulation::remove_non_static_object(std::shared_ptr<sf::Shape> shape)
{
    for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++) {
        if (nonStaticObjects[i] == shape) {
            nonStaticObjects.erase(nonStaticObjects.begin() + i);
            break;
        }
    }
}

