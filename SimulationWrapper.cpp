#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>
#include "CollisionShapes.hpp"
// g++ test.cpp -o test.o -lsfml-graphics -lsfml-window -lsfml-system

class Window : public sf::RenderWindow
{
    private:
        void applyGravity()
        {
            for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++)
            {
                auto shape = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);
                shape->accelerate(gravity);
            }
        }
        void applyCircularConstraint()
        {
            const sf::Vector2f center = sf::Vector2f(this->getSize().x / 2.0f, this->getSize().y / 2.0f);
            const float radius = this->getSize().y/2.0f;
            // All calculations are about the center of the object
            for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++)
            {
                auto shape = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);
                sf::Vector2f position = shape->getPosition();
                sf::Vector2f direction = position - center;

                // Distance from center
                float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

                if (distance > radius-shape->getRadius())
                {
                    std::cout << "Position0: " << shape->position_current.x << ", " << shape->position_current.y << std::endl;
                    direction = direction / distance;
                    shape->setPositionCurrent(center + direction * (radius-shape->getRadius()) * .999f);

                    std::cout << "Position1: " << shape->position_current.x << ", " << shape->position_current.y << std::endl;
                    
                    // TODO: fix bounce
                    /*
                    Position0: 500, 529.911
                    Position1: 499.714, 529.253
                    Position2: 510.061, 537.783
                    Position0: 168.604, 400.191
                    Position1: 170.582, 399.335
                    Position2: 166.022, 405.6
                    Position0: 457.904, 554.019
                    Position1: 455.562, 543.747
                    Position2: 453.811, 545.17
                    */
                    // Bounce
                    sf::Vector2f v = shape->position_current - shape->position_old;
                    sf::Vector2f v_normal = direction * (v.x * direction.x + v.y * direction.y);
                    sf::Vector2f v_tangent = v - v_normal;
                    sf::Vector2f v_reflected = v_tangent - v_normal * 0.9f; 

                    shape->position_old = shape->position_current - v_reflected;

                    std::cout << "Position2: " << shape->position_old.x << ", " << shape->position_old.y << std::endl;
                    
                }
            }
        }
    public:
        sf::Vector2f gravity = sf::Vector2f(0, 981.0f);
        std::vector<std::shared_ptr<sf::Shape>> shapes;
        std::vector<std::shared_ptr<sf::Shape>> nonStaticObjects;
        std::vector<std::shared_ptr<sf::Text>> TextInputFields;
        std::vector<bool> isTextInputFieldCentered;
        std::vector<std::shared_ptr<sf::Shape>> boundingBoxes;
        std::vector<bool> isTextInputActive;
        bool isFocused = false;

        // Contructor that inherets from sf::RenderWindow
        Window(sf::VideoMode mode, const sf::String &title, sf::Uint32 style = sf::Style::Default, const int frameRate = 60,
                const sf::ContextSettings &settings = sf::ContextSettings(), const float posRatioX = 0.5, const float posRatioY = 0.5, bool resizable = true)
            : sf::RenderWindow(mode, title, style, settings)
        {
            // Get the desktop resolution
            sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
            // Get the window position
            sf::Vector2i windowPos;
            windowPos.x = desktop.width * posRatioX;
            windowPos.y = desktop.height * posRatioY;
            // Set the window position
            this->setPosition(windowPos);

            // Set the framerate limit
            this->setFramerateLimit(frameRate);

            // Set the window to be active
            this->setActive(true);

            // Set the window to be focused
            this->requestFocus();

            // Set the window to be on top
            this->setVerticalSyncEnabled(true);

            // Set the window to be resizable
            this->setVerticalSyncEnabled(resizable);

            // Test constraint
            if (this->getSize().x > this->getSize().y)
            {
                const sf::Vector2f center = sf::Vector2f(this->getSize().x / 2.0f, this->getSize().y / 2.0f);
                const float radius = this->getSize().y/2.0f;
                sf::CircleShape circularArea(radius);
                circularArea.setFillColor(sf::Color::White); // Transparent fill
                circularArea.setOutlineColor(sf::Color::Yellow); // Outline color
                circularArea.setOutlineThickness(5.f); // Outline thickness
                circularArea.setOrigin(radius, radius);
                circularArea.setPosition(center);
                shapes.push_back(std::make_shared<sf::CircleShape>(circularArea));
            }
        }

        
        void draw(const sf::Drawable &drawable, const sf::RenderStates &states = sf::RenderStates::Default)
        {
            sf::RenderWindow::draw(drawable, states);
        }
        
        void update()
        {       
            this->clear();
            for (int i = 0; i < static_cast<int>(shapes.size()); i++)
            {
                this->draw(*shapes[i]);
            }
            for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++)
            {
                this->draw(*nonStaticObjects[i]);
            }
            for (int i = 0; i < static_cast<int>(TextInputFields.size()); i++)
            {
                if (isTextInputFieldCentered[i])
                {
                    float x_centered = boundingBoxes[i]->getPosition().x + boundingBoxes[i]->getGlobalBounds().width/2 - TextInputFields[i]->getGlobalBounds().width/2;
                    float y_centered = boundingBoxes[i]->getPosition().y;
                    TextInputFields[i]->setPosition(x_centered,y_centered);
                }
                this->draw(*boundingBoxes[i]);
                this->draw(*TextInputFields[i]);
            }
            this->display();
        }

        void updatePosNONStatic(float dt)
        {
            applyGravity();
            applyCircularConstraint();
            for (int i = 0; i < static_cast<int>(nonStaticObjects.size()); i++)
            {
                auto shape = std::dynamic_pointer_cast<Ball>(nonStaticObjects[i]);
                shape->updatePos(dt);
                //std::cout << "Position: " << shape->getPosition().x << ", " << shape->getPosition().y << std::endl;
            }
        }

        void addShape(std::shared_ptr<sf::Shape> shape)
        {
            shapes.push_back(shape);
        }

        void addNonStaticObject(std::shared_ptr<sf::Shape> shape)
        {
            nonStaticObjects.push_back(shape);
        }

        void addTextInputField(std::shared_ptr<sf::Text> text, float width = 100, float height = 4, bool centered = false)
        {
            // Create a bounding box for the text
            sf::FloatRect textBounds = text->getGlobalBounds();
            auto boundingBox = std::make_shared<sf::RectangleShape>(sf::Vector2f(textBounds.width+width, textBounds.height+height));
            boundingBox->setPosition(textBounds.left-1, textBounds.top+height/2);
            boundingBox->setFillColor(sf::Color::Transparent);
            boundingBox->setOutlineColor(sf::Color::White);
            boundingBox->setOutlineThickness(1.f);

            if (centered)
            {
                text->setPosition(boundingBox->getPosition().x + boundingBox->getGlobalBounds().width/2 - text->getGlobalBounds().width/2, boundingBox->getPosition().y);
            }

            // Add the bounding box and text to the window
            boundingBoxes.push_back(boundingBox);
            TextInputFields.push_back(text);
            isTextInputFieldCentered.push_back(centered);
            isTextInputActive.push_back(false);
        }

        void removeDrawable(std::shared_ptr<sf::Shape> shape)
        {
            for (int i = 0; i < static_cast<int>(shapes.size()); i++)
            {
                if (shapes[i] == shape)
                {
                    shapes.erase(shapes.begin() + i);
                    break;
                }
            }
        }

        void removeTextInputField(std::shared_ptr<sf::Text> text)
        {
            for (int i = 0; i < static_cast<int>(TextInputFields.size()); i++)
            {
                if (TextInputFields[i] == text)
                {
                    TextInputFields.erase(TextInputFields.begin() + i);
                    boundingBoxes.erase(boundingBoxes.begin() + i);
                    isTextInputFieldCentered.erase(isTextInputFieldCentered.begin() + i);
                    isTextInputActive.erase(isTextInputActive.begin() + i);
                    break;
                }
            }
        }

        std::vector<std::shared_ptr<sf::Shape>> getShapes()
        {
            return shapes;
        }

        std::vector<std::shared_ptr<sf::Text>> getTextInputFields()
        {
            return TextInputFields;
        }

        std::vector<std::shared_ptr<sf::Shape>> getBoundingBoxes()
        {
            return boundingBoxes;
        }

        void checkInputTextSelected(sf::Vector2f mousePos)
        {
            for (int i = 0; i < static_cast<int>(boundingBoxes.size()); i++)
            {
                if (boundingBoxes[i]->getGlobalBounds().contains(mousePos))
                {
                    isTextInputActive[i] = true;
                }
                else
                {
                    isTextInputActive[i] = false;
                }
            }
        }

        void setAllTextInputFieldsInactive()
        {
            for (int i = 0; i < static_cast<int>(isTextInputActive.size()); i++)
            {
                isTextInputActive[i] = false;
            }
        }

        std::shared_ptr<sf::Text> getActiveTextInputField()
        {
            for (int i = 0; i < static_cast<int>(isTextInputActive.size()); i++)
            {
                if (isTextInputActive[i])
                {
                    return TextInputFields[i];
                }
            }
            return nullptr;
        }

};

int main()
{
    const std::vector<unsigned int> ACCEPTABLE_UNICODE = {8, 32, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 46};
    const float TIME_SUB_STEPS = 8.0f;
    const float GRAVITY = 9.81f;

    Window window1(sf::VideoMode(800, 600), "Physics Simulator", sf::Style::Close, 60, sf::ContextSettings(), 0.1, 0.25, true);
    Window window2(sf::VideoMode(300, 600), "Configuration", sf::Style::Close, 60, sf::ContextSettings(), 0.6, 0.25, true);

    // Window 1 add shapes
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(shape.getRadius()+10, shape.getRadius()+10);
    window1.addShape(std::make_shared<sf::CircleShape>(shape));
    sf::Vector2f velocity = sf::Vector2f(0.0f, 0.0f);
    int mass = 25;

    sf::RectangleShape rectangle(sf::Vector2f(120, 5));
    rectangle.setFillColor(sf::Color::Red);
    rectangle.setPosition(10, 10);
    window1.addShape(std::make_shared<sf::RectangleShape>(rectangle));

    Ball ball(50.f, 30, sf::Vector2f(500, 95), 1, sf::Color::Blue, sf::Color::White);
    window1.addNonStaticObject(std::make_shared<Ball>(ball));


    // Window 2 add Text Input Fields
    std::string font_path = "./arial.ttf";
    sf::String playerInput = "000";
    sf::Font font;
    if (!font.loadFromFile(font_path)) {
        std::cerr << "Error loading font\n";
        return -1;
    }
    sf::Text playerText;
    playerText.setFont(font);
    playerText.setString(playerInput);
    playerText.setCharacterSize(24);
    playerText.setFillColor(sf::Color::Red);
    playerText.setPosition(100-playerText.getGlobalBounds().width/2,5);

    window2.addTextInputField(std::make_shared<sf::Text>(playerText), 100, 8, true);

    window1.isFocused = false;
    window2.isFocused = true;

    sf::Clock clock;
    float accumulator = 0.f;

    while (window1.isOpen() || window2.isOpen())
    {
        sf::Event event;
        while (window1.pollEvent(event))
        {
            if (event.type == sf::Event::GainedFocus) 
            {
                window1.isFocused = true;
                break;
            }

            if (!window1.isFocused)
            {
                break;
            }

            if (event.type == sf::Event::LostFocus) 
            {
                window1.isFocused = false;
            }
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
            {
                window1.close();
                break;
            }

        }
        while (window2.pollEvent(event))
        {

            if (event.type == sf::Event::GainedFocus) 
            {
                window2.isFocused = true;
                break;
            }

            if (!window2.isFocused)
            {
                break;
            }

            if (event.type == sf::Event::LostFocus) 
            {
                window2.isFocused = false;
            }
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
            {
                window2.close();
                break;
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos = window2.mapPixelToCoords(sf::Mouse::getPosition(window2));
                    window2.checkInputTextSelected(mousePos);
                }
            }

            if (event.type == sf::Event::TextEntered)
            {

                if (std::find(ACCEPTABLE_UNICODE.begin(), ACCEPTABLE_UNICODE.end(), event.text.unicode) != ACCEPTABLE_UNICODE.end())
                {
                    auto activeTextInputField = window2.getActiveTextInputField();
                    if (activeTextInputField == nullptr)
                    {
                        continue;
                    }

                    playerInput = activeTextInputField->getString();


                    if (event.text.unicode == 8 && !playerInput.isEmpty()) // 8 is the ASCII code for backspace
                    {
                        playerInput.erase(playerInput.getSize() - 1, 1);

                        if (playerInput.isEmpty())
                        {
                            playerInput = "0";
                        }
                    }
                    else if (event.text.unicode != 8)
                    {
                        if (playerInput.getSize() == 1 && playerInput[0] == '0' && event.text.unicode != '0' && event.text.unicode != '.') {
                            playerInput.erase(0, 1);
                        }

                        playerInput += static_cast<char>(event.text.unicode);
                    }
                    activeTextInputField -> setString(playerInput);

                    // Convert the player input to a float
                    float playerInputValue = 0;
                    try {
                        playerInputValue = std::stof(playerInput.toAnsiString());
                        //std::cout << "Player input as float: " << playerInputValue << std::endl;
                    } catch (const std::invalid_argument& e) {
                        continue;
                    } catch (const std::out_of_range& e) {
                        continue;
                    }
                    //std::cout << window2.getTextInputFields()[0]->getString().toAnsiString() << std::endl;

                    if (playerInputValue > 100) {
                        playerInputValue = 100;
                    }
                    else if (playerInputValue < 0) {
                        playerInputValue = 1;
                    }
                    
                    if (playerInputValue != 0)
                    {
                        
                        // get the old radius and position of the circle
                        float oldRadius = std::dynamic_pointer_cast<sf::CircleShape>(window1.getShapes()[0])->getRadius();
                        sf::Vector2f oldPosition = std::dynamic_pointer_cast<sf::CircleShape>(window1.getShapes()[0])->getPosition();
                        sf::Vector2f oldCenter = oldPosition + sf::Vector2f(oldRadius, oldRadius);

                        // Change position so the center point stays the same when radius changes
                        sf::Vector2f newPosition = oldCenter - sf::Vector2f(playerInputValue, playerInputValue);
                        

                        // Dynamic cast the shape to a circle and change the radius
                        std::dynamic_pointer_cast<sf::CircleShape>(window1.getShapes()[0])->setPosition(newPosition);
                        std::dynamic_pointer_cast<sf::CircleShape>(window1.getShapes()[0])->setRadius(playerInputValue);
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Return)
                {
                    window2.setAllTextInputFieldsInactive();
                }
            }
            
        }

        float dt = clock.restart().asSeconds();
        accumulator += dt;

        while (accumulator >= 1.f / 60.0f)
        {
            window1.updatePosNONStatic(1.f / 60.0f);
            accumulator -= 1.f / 60.0f;
        }
        

        window1.update();
        window2.update();
    }

    return 0;
}