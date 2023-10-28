#pragma once
#include <SFML/Graphics.hpp>
#include <string>  // You should include the string header.

class BUTTON {
public:
    BUTTON(std::string napis, int x, int y);  // Use std::string instead of string.
    // Declare functions and methods related to the button here
    void draw(sf::RenderWindow& window);
    int pos_x;
    int pos_y;
    sf::Sprite sprite;
    sf::FloatRect getSpriteGlobalBounds() {
        return sprite.getGlobalBounds();
    }

private:
    sf::Font font;
    sf::Color szary;
    sf::Texture texture;
    sf::Text text;
    sf::RectangleShape button;
};



