#include <SFML/Graphics.hpp>
#include <memory>
#include <cmath>
#include "../../include/ConfigurationWindow.hpp"


/////////CONSTRUCTOR/////////
windowConfig::windowConfig(const sf::VideoMode &videoMode, const sf::String &title, sf::Uint32 style, 
                const sf::ContextSettings &settings, const float posRatioX, const float posRatioY, 
                bool resizable, bool isFocused, int frameRate, sf::Color backgroundColor = sf::Color::Black) 
                : sf::RenderWindow(videoMode, title, style, settings) // Inherit from sf::RenderWindow
{
    this->setFramerateLimit(frameRate);
    this->setActive(true);

    if (isFocused)
        this->requestFocus();
    this->isFocused = isFocused;

    this->setVerticalSyncEnabled(resizable);

    // Set the window position
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int monitorWidth = desktop.width;
    int monitorHeight = desktop.height;
    this->setPosition(sf::Vector2i(monitorWidth * posRatioX, monitorHeight * posRatioY));

    this->backgroundColor = backgroundColor;

}

/////////DESTRUCTOR/////////
windowConfig::~windowConfig() 
{
    //delete this;
}

/////////FUNCTIONS/////////
void windowConfig::draw(const sf::Drawable &drawable, const sf::RenderStates &states = sf::RenderStates::Default)
{
    this->sf::RenderWindow::draw(drawable, states);
}

void windowConfig::update_visuals()
{
    this->clear(backgroundColor);
    for (int i = 0; i < static_cast<int>(textInputFields.size()); i++) {
        if (textInputConfig[i][1]) {
            if (textInputConfig[i][0]) {
                float x_centered = textBoundingBoxes[i]->getPosition().x + textBoundingBoxes[i]->getGlobalBounds().width/2 - textInputFields[i]->getGlobalBounds().width/2;
                float y_centered = textBoundingBoxes[i]->getPosition().y;
                textInputFields[i]->setPosition(x_centered,y_centered);
            }
            this->draw(*textBoundingBoxes[i]);
            this->draw(*textInputFields[i]);
        }
    }
    this->display();
}

void windowConfig::add_text_input_field(std::shared_ptr<sf::Text> text, float width, float height, bool centered)
{
    // Create a bounding box for the text
    sf::FloatRect textBounds = text->getGlobalBounds();
    auto boundingBox = std::make_shared<sf::RectangleShape>(sf::Vector2f(textBounds.width+width, textBounds.height+height));
    boundingBox->setPosition(textBounds.left-1, textBounds.top+height/2);
    boundingBox->setFillColor(sf::Color::Transparent);
    boundingBox->setOutlineColor(sf::Color::White);
    boundingBox->setOutlineThickness(1.f);

    textInputFields.push_back(text);
    textBoundingBoxes.push_back(boundingBox);
    textInputConfig.push_back({centered, false});
}

void windowConfig::remove_text_input_field(std::shared_ptr<sf::Text> text)
{
    for (int i = 0; i < static_cast<int>(textInputFields.size()); i++) {
        if (textInputFields[i] == text) {
            textInputFields.erase(textInputFields.begin() + i);
            textBoundingBoxes.erase(textBoundingBoxes.begin() + i);
            textInputConfig.erase(textInputConfig.begin() + i);
            break;
        }
    }
}

void windowConfig::check_input_text_selected(sf::Vector2f mousePos)
{
    for (int i = 0; i < static_cast<int>(textInputFields.size()); i++) {
        if (textBoundingBoxes[i]->getGlobalBounds().contains(mousePos)) {
            textInputConfig[i][1] = true;
        }
        else {
            textInputConfig[i][1] = false;
        }
    }
}

void windowConfig::set_all_text_input_fields_inactive()
{
    for (int i = 0; i < static_cast<int>(textInputFields.size()); i++) {
        textInputConfig[i][1] = false;
    }
}

std::shared_ptr<sf::Text> windowConfig::get_active_text_input_field()
{
    for (int i = 0; i < static_cast<int>(textInputFields.size()); i++) {
        if (textInputConfig[i][1]) {
            return textInputFields[i];
        }
    }
    return nullptr;
}

