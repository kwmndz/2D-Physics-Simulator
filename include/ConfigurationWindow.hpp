#ifndef CONFIGURATION_WINDOW_HPP
#define CONFIGURATION_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>

class windowConfig : public sf::RenderWindow
{
    private:
    public:
        /////////PUBLIC_VARIABLES/////////
        std::vector<std::shared_ptr<sf::Text>> textInputFields;
        std::vector<std::vector<bool>> textInputConfig; // [0] = isCentered, [1] = isActive
        std::vector<std::shared_ptr<sf::Shape>> textBoundingBoxes;
        std::vector<unsigned int> textInputAttachedObjects; // Index of the object the text input is attached to (-1 if none)
        bool isFocused = false;
        sf::Color backgroundColor;

        /////////CONSTRUCTOR/////////
        windowConfig(const sf::VideoMode &videoMode, const sf::String &title, sf::Uint32 style, 
                const sf::ContextSettings &settings, const float posRatioX, const float posRatioY, 
                bool resizable, bool isFocused, int frameRate, sf::Color backgroundColor);
        
        /////////DESTRUCTOR/////////
        ~windowConfig();

        /////////FUNCTIONS/////////
        void draw(const sf::Drawable &drawable, const sf::RenderStates &states);
        void update_visuals();
        void add_text_input_field(std::shared_ptr<sf::Text> text, float width, float height, bool centered, unsigned int attachedObject);
        void remove_text_input_field(std::shared_ptr<sf::Text> text);
        void check_input_text_selected(sf::Vector2f mousePos);
        void set_all_text_input_fields_inactive();
        std::shared_ptr<sf::Text> get_active_text_input_field();
        int get_active_attached_object_index();
};


#endif // ConfigurationWindow.hpp 