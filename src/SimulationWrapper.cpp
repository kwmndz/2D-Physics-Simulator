#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>
#include "../include/CollisionShapes.hpp"
#include "../include/ConfigurationWindow.hpp"
#include "../include/SimulationWindow.hpp"

int main()
{
    const std::vector<unsigned int> ACCEPTABLE_UNICODE = {8, 32, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 46};
    const float TIME_SUB_STEPS = 8.0f;
    const float GRAVITY = 9.81f;

    // Constraint:
    sf::CircleShape constraingCircle(300.f);
    constraingCircle.setFillColor(sf::Color::White);
    constraingCircle.setOrigin(300.f, 300.f);
    constraingCircle.setPosition(sf::Vector2f(400, 300));

    std::shared_ptr<sf::Shape> constraint = std::make_shared<sf::CircleShape>(constraingCircle);

    windowSimulation simWin1(sf::VideoMode(800, 600), "Physics Simulator", sf::Style::Close, sf::ContextSettings(), 0.1, 0.25, true, true, 60, constraint, sf::Color::Black);
    windowConfig configWin1(sf::VideoMode(300, 600), "Configuration", sf::Style::Close, sf::ContextSettings(), 0.6, 0.25, true, false, 60, sf::Color::Black);

    // Sim Window config:
    /*
    Ball ball(50.f, 30, sf::Vector2f(500, 95), 1, sf::Color::Blue, sf::Color::White);
    Ball ball2(50.f, 30, sf::Vector2f(150, 300), 1, sf::Color::Red, sf::Color::White);
    Ball ball3(50.f, 30, sf::Vector2f(200, 50), 1, sf::Color::Green, sf::Color::White);
    simWin1.add_non_static_object(std::make_shared<Ball>(ball));
    simWin1.add_non_static_object(std::make_shared<Ball>(ball2));
    simWin1.add_non_static_object(std::make_shared<Ball>(ball3));
    */

    // Config Window config:
    std::string font_path = "./arial.ttf";
    sf::String playerInput = "0";
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
    configWin1.add_text_input_field(std::make_shared<sf::Text>(playerText), 100, 8, true);

    // Main loop

    sf::Clock clock;
    float accumulator = 0.f;
    float acc2 = 0.f;

    simWin1.update_visuals();
    configWin1.update_visuals();

    sf::sleep(sf::seconds(1));


    const float radius = 10.f;
    const int numColors = 10;
    std::vector<sf::Color> colors = {
        sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta,
        sf::Color::Cyan, sf::Color(255, 165, 0), sf::Color::Black, sf::Color(255, 165, 0), sf::Color(128, 0, 128)
    };
    int count = 0;

    while (simWin1.isOpen() || configWin1.isOpen()) 
    {
        sf::Event event;
        while (simWin1.pollEvent(event)) {
            if (event.type == sf::Event::GainedFocus) {
                simWin1.isFocused = true;
                break;
            }

            if (!simWin1.isFocused) {
                break;
            }

            if (event.type == sf::Event::LostFocus) {
                simWin1.isFocused = false;
            }
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) {
                simWin1.close();
                break;
            }

        }
        while (configWin1.pollEvent(event)) {

            if (event.type == sf::Event::GainedFocus) {
                configWin1.isFocused = true;
                break;
            }

            if (!configWin1.isFocused) {
                break;
            }

            if (event.type == sf::Event::LostFocus) {
                configWin1.isFocused = false;
            }
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) {
                configWin1.close();
                break;
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = configWin1.mapPixelToCoords(sf::Mouse::getPosition(configWin1));
                    configWin1.check_input_text_selected(mousePos);
                }
            }

            if (event.type == sf::Event::TextEntered) {

                if (std::find(ACCEPTABLE_UNICODE.begin(), ACCEPTABLE_UNICODE.end(), event.text.unicode) != ACCEPTABLE_UNICODE.end()) {
                    auto activeTextInputField = configWin1.get_active_text_input_field();
                    if (activeTextInputField == nullptr) {
                        continue;
                    }

                    playerInput = activeTextInputField->getString();


                    if (event.text.unicode == 8 && !playerInput.isEmpty()) { // 8 is the ASCII code for backspace
                        playerInput.erase(playerInput.getSize() - 1, 1);

                        if (playerInput.isEmpty()) {
                            playerInput = "0";
                        }
                    }
                    else if (event.text.unicode != 8) {
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
                    
                    if (playerInputValue != 0) {
                        
                        // TODO: Attach inputs to certain objects/constants
                        continue;
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Return) {
                    configWin1.set_all_text_input_fields_inactive();
                }
            }
            
        }

        float dt = 0.016;//clock.restart().asSeconds();
        accumulator += dt;
        acc2 += dt;
        float sub_dt = 0;

        /*
        while (accumulator >= 1.f / 60.0f) {
            sub_dt = 1.f / 60.0f / TIME_SUB_STEPS;
            for (int i = 0; i < 8; i++) {
                simWin1.update_positions(sub_dt);
            }
            accumulator -= 1.f / 60.0f;
        }
        */
        
        sub_dt = dt/ TIME_SUB_STEPS;
        for (int i = 0; i < 8; i++) {
            simWin1.update_positions(sub_dt);
        }

        if (acc2 >= 0.1 and count < 3) {
            float xPos = 400 + 48 * std::cos(count * M_PI / 2); // Oscillate between 476 and 524

            Ball tinyBall(radius, 30, sf::Vector2f(xPos, 80), 1, colors[count%10], sf::Color::White);
            simWin1.add_non_static_object(std::make_shared<Ball>(tinyBall));
            acc2 = 0;
            count++;
        }

        // Check for mouse hover over balls
        sf::Vector2i mousePos = sf::Mouse::getPosition(simWin1);
        for (const auto& ball : simWin1.nonStaticObjects) {
            if (ball->getGlobalBounds().contains(simWin1.mapPixelToCoords(mousePos))) {
            auto it = std::find(simWin1.nonStaticObjects.begin(), simWin1.nonStaticObjects.end(), ball);
            if (it != simWin1.nonStaticObjects.end()) {
                int index = std::distance(simWin1.nonStaticObjects.begin(), it);
                std::cout << "Ball index: " << index << std::endl;
            }
            break;
            }
        }
        
        /*
        while (accumulator >= 1.f / 60.0f) {
            simWin1.update_positions(1.f / 60.0f);
            accumulator -= 1.f / 60.0f;
        }*/
        

        simWin1.update_visuals();
        configWin1.update_visuals();
    }

    return 0;
}