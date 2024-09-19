#ifndef SIMULATION_WINDOW_HPP
#define SIMULATION_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>

class windowSimulation : public sf::RenderWindow
{
    private:
        /////////PRIVATE_VARIABLES/////////
        std::shared_ptr<sf::Shape> ConstraintShape;

        /////////HELPER_FUNCTIONS/////////
        void applyGravity();
        void applyCircularConstraint();
        void applyCollision();
    public:
        /////////PUBLIC_VARIABLES/////////
        sf::Vector2f gravity = sf::Vector2f(0, 981.0f);
        std::vector<std::shared_ptr<sf::Shape>> shapes;
        std::vector<std::shared_ptr<sf::Shape>> nonStaticObjects;
        bool isFocused = false;
        sf::Color backgroundColor;

        /////////CONSTRUCTOR/////////
        windowSimulation(const sf::VideoMode &videoMode, const sf::String &title, sf::Uint32 style, 
                const sf::ContextSettings &settings, const float posRatioX, const float posRatioY, 
                bool resizable, bool isFocused, int frameRate, std::shared_ptr<sf::Shape> constraint, sf::Color backgroundColor);
        
        /////////DESTRUCTOR/////////
        ~windowSimulation();

        /////////FUNCTIONS/////////
        void draw(const sf::Drawable &drawable, const sf::RenderStates &states);
        void update_visuals();
        void update_positions(float dt);
        void add_shape(std::shared_ptr<sf::Shape> shape);
        void add_non_static_object(std::shared_ptr<sf::Shape> shape);
        void remove_shape(std::shared_ptr<sf::Shape> shape);
        void remove_non_static_object(std::shared_ptr<sf::Shape> shape);

};

#endif // SimulationWindow_HPP