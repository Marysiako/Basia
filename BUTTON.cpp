#include <SFML/Graphics.hpp>
#include <string>  // You should include the string header.
#include"BUTTON.h"
#include <iostream>

BUTTON::BUTTON(std::string napis, int x, int y) {
    // Initialize sprite, text, and button in the constructor.
    if (!font.loadFromFile("minecraft_font.ttf")) {
        std::cout << "Nie zaladowano czcionki\n";
    }
    szary = sf::Color(105, 105, 105);

    if (!texture.loadFromFile("graphic/button.png")) {
        std::cout << "Nie zaladowano grafiki\n";
    }
    pos_x = x;
    pos_y = y;
    sprite.setTexture(texture);
    sprite.setPosition(pos_x, pos_y);

    text.setFillColor(szary);
    text.setCharacterSize(40);
    text.setFont(font);
    text.setString(napis);
    text.setPosition(pos_x + 60, pos_y + 30);

    button.setSize(sf::Vector2f(256, 100));
    button.setPosition(pos_x, pos_y);
}


void BUTTON::draw(sf::RenderWindow& window) {
    window.draw(button);
    window.draw(sprite);
    window.draw(text);
}

