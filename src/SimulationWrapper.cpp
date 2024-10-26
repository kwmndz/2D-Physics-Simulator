#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include "../include/CollisionShapes.hpp"
#include "../include/ConfigurationWindow.hpp"
#include "../include/SimulationWindow.hpp"

// * Saves the indices of the balls that have been colored by left clicking
void saveToFile(std::string filename, std::vector<int> ballIndicesColored);

// * Loads the indices of the balls
void loadFromFile(std::string filename, std::vector<int>& ballIndicesColored);

// * Compiles the inputs from the command line
bool compileInputs(std::string& output_file, std::string& input_file, int& color, int& pcolor, int argc, char** argv);

// * Returns sf color object based on the input color
sf::Color getColor(int color);

int main(int argc, char** argv)
{
    std::string output_file = "";
    std::string input_file = "";
    int color = -1;
    int pcolor = 0;

    if (argc > 10) {
        std::cerr << "Usage: -output <output_file> -input <input_file> -color <color> -pcolor <color>\n";
        std::cerr << "Colors: -1 = random, 0 = black, 1 = white, 2 = red, 3 = green, 4 = blue, 5 = yellow, 6 = magenta, 7 = cyan, 8 = orange, 9 = purple\n";
        //// std::cout << argc << std::endl;
        return -1;
    }

    if (compileInputs(output_file, input_file, color, pcolor, argc, argv)) {
        return 0;
    }

    const std::vector<unsigned int> ACCEPTABLE_UNICODE = {8, 32, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 46};
    const float TIME_SUB_STEPS = 8.0f;
    const float GRAVITY = 9.81f;

    // Constraint:
    sf::CircleShape constraingCircle(300.f);
    constraingCircle.setPointCount(100);
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
    sf::String playerInput = "981.0";
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
    configWin1.add_text_input_field(std::make_shared<sf::Text>(playerText), 100, 8, true, 0);


    // * Main loop

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
        sf::Color::Cyan, sf::Color(255, 165, 0), sf::Color(129, 11, 233), sf::Color(255, 165, 0), sf::Color(128, 0, 128)
    };
    int count = 0;

    bool isLeftClicked = false;
    sf::Color leftClickColor = getColor(pcolor);

    std::vector<int> ballIndicesColored;

    loadFromFile(input_file, ballIndicesColored);

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
                saveToFile(output_file, ballIndicesColored);
                if (output_file != "") {
                    std::cout << "Saved to file: " << output_file << std::endl;
                }
                simWin1.close();
                break;
            }

            if(event.type == sf::Event::MouseButtonPressed and event.mouseButton.button == sf::Mouse::Left) {
                isLeftClicked = true;
            }

            if(event.type == sf::Event::MouseButtonReleased and event.mouseButton.button == sf::Mouse::Left) {
                isLeftClicked = false;
            }

            if (isLeftClicked) {
                sf::Vector2f mousePos = simWin1.mapPixelToCoords(sf::Mouse::getPosition(simWin1));
                for (const auto& ball : simWin1.nonStaticObjects) {
                    if (ball->getGlobalBounds().contains(mousePos) && ball->getFillColor() != leftClickColor) {
                        auto it = std::find(simWin1.nonStaticObjects.begin(), simWin1.nonStaticObjects.end(), ball);
                        if (it != simWin1.nonStaticObjects.end()) {
                            int index = std::distance(simWin1.nonStaticObjects.begin(), it);
                            //// std::cout << "Ball index: " << index << std::endl;
                            ballIndicesColored.push_back(index);
                        }
                        ball->setFillColor(leftClickColor);
                        break;
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                sf::Vector2f mousePos = simWin1.mapPixelToCoords(sf::Mouse::getPosition(simWin1));
                Ball tinyBall(radius, 30, mousePos, 1, colors[count % numColors], sf::Color::White);
                if (color != -1) {
                    tinyBall.setFillColor(getColor(color));
                }
                simWin1.add_non_static_object(std::make_shared<Ball>(tinyBall));
                count++;
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
                        ////std::cout << "Player input as float: " << playerInputValue << std::endl;
                    } catch (const std::invalid_argument& e) {
                        continue;
                    } catch (const std::out_of_range& e) {
                        continue;
                    }
                    ////std::cout << window2.getTextInputFields()[0]->getString().toAnsiString() << std::endl;

                    if (playerInputValue > 100) {
                        playerInputValue = 100;
                    }
                    else if (playerInputValue < 0) {
                        playerInputValue = 1;
                    }
                    
                    if (playerInputValue != 0) {
                        
                        // TODO: Attach inputs to certain objects/constants
                        ////continue;
                        int activeAttachedObjectIndex = configWin1.get_active_attached_object_index();
                        if (activeAttachedObjectIndex == 0) {
                            simWin1.gravity = sf::Vector2f(0, playerInputValue);
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Return) {
                    configWin1.set_all_text_input_fields_inactive();
                }
            }
            
        }


        if (acc2 >= 0.01 and count < 500) {
            float angle = count * (M_PI / 4); // Rotate by 45 degrees for each ball
            float xPos = 400 + 100 * std::cos(angle); 
            ////float yPos = 300 + 100 * std::sin(angle);
            Ball tinyBall(radius, 30, sf::Vector2f(xPos, 56), 1, colors[count % numColors], sf::Color::White);
            if ( std::find(ballIndicesColored.begin(), ballIndicesColored.end(), count) != ballIndicesColored.end()) {
                tinyBall.setFillColor(leftClickColor);
            } else if(color != -1) {
                tinyBall.setFillColor(getColor(color));
            }
            ////tinyBall.accelerate(sf::Vector2f(100 * std::cos(angle), 100 * std::sin(angle))); // Shoot out in rotating directions
            simWin1.add_non_static_object(std::make_shared<Ball>(tinyBall));
            simWin1.apply_acceleration(sf::Vector2f(100000 * std::cos(angle), 100000 * 2), simWin1.nonStaticObjects.size() - 1);
            acc2 = 0;
            count++;
        }

        float dt = 0.016;////clock.restart().asSeconds();
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

void saveToFile(std::string filename, std::vector<int> ballIndicesColored) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& index : ballIndicesColored) {
            if (index == ballIndicesColored.back()) {
                outFile << index;
                break;
            }
            outFile << index << ",";
        }
        outFile.close();
    } else {
        std::cerr << "Unable to open file for saving indecies\n";
    }
}

void loadFromFile(std::string filename, std::vector<int>& ballIndicesColored) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::string line;
        std::getline(inFile, line);
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            //// std::cout << "Token: " << token << std::endl;
            ballIndicesColored.push_back(std::stoi(token));
        }
        inFile.close();
    } else {
        std::cerr << "Unable to open file for loading indecies\n";
    }
}

bool compileInputs(std::string& output_file, std::string& input_file, int& color, int& pcolor, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--help") {
            std::cout << "Usage: -output <output_file> -input <input_file> -color <color> -pcolor <color>\n";
            std::cout << "Colors: -1 = random, 0 = black, 1 = white, 2 = red, 3 = green, 4 = blue, 5 = yellow, 6 = magenta, 7 = cyan, 8 = orange, 9 = purple\n";
            return true;
        } else if (std::string(argv[i]) == "-output") {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                std::cout << "Output file: " << output_file << std::endl;
            }
        } else if (std::string(argv[i]) == "-input") {
            if (i + 1 < argc) {
                input_file = argv[i + 1];
                std::cout << "Input file: " << input_file << std::endl;
            }
        } else if (std::string(argv[i]) == "-color") {
            if (i + 1 < argc) {
                color = std::stoi(argv[i + 1]);
                std::cout << "Color: " << color << std::endl;
            }
        } else if (std::string(argv[i]) == "-pcolor") {
            if (i + 1 < argc) {
                pcolor = std::stoi(argv[i + 1]);
                std::cout << "PColor: " << pcolor << std::endl;
            }
        }
    }
    return false;
}

sf::Color getColor(int color) {
    switch (color) {
        case -1:
            return sf::Color(rand() % 256, rand() % 256, rand() % 256);
        case 0:
            return sf::Color::Black;
        case 1:
            return sf::Color::White;
        case 2:
            return sf::Color::Red;
        case 3:
            return sf::Color::Green;
        case 4:
            return sf::Color::Blue;
        case 5:
            return sf::Color::Yellow;
        case 6:
            return sf::Color::Magenta;
        case 7:
            return sf::Color::Cyan;
        case 8:
            return sf::Color(255, 165, 0);
        case 9:
            return sf::Color(128, 0, 128);
        default:
            return sf::Color::Black;
    }
}