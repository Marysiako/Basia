#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class BUTTON {
public:
    BUTTON(std::string napis, int x, int y, std::string type);  // Use std::string instead of string.
    // Declare functions and methods related to the button here
    void draw(sf::RenderWindow& window);
    void changeText(std::string newText);
    int pos_x;
    int pos_y;
    sf::Sprite sprite;
    sf::FloatRect getSpriteGlobalBounds() {
        return sprite.getGlobalBounds();
    }
    sf::Text text;
    std::string button_string;

private:
    sf::Font font;
    sf::Color szary;
    sf::Texture texture;
    
    sf::RectangleShape button;
};



