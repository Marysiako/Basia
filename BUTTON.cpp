#include <SFML/Graphics.hpp>
#include <string>  // You should include the string header.
#include"BUTTON.h"
#include <iostream>

BUTTON::BUTTON(std::string napis, int x, int y, std::string type) {
    // Initialize sprite, text, and button in the constructor.
    if (!font.loadFromFile("minecraft_font.ttf")) {
        std::cout << "Nie zaladowano czcionki\n";
    }
    szary = sf::Color(105, 105, 105);

    if (type == "big")
    {
        if (!texture.loadFromFile("graphic/button.png")) {
            std::cout << "Nie zaladowano grafiki\n";
        }
        pos_x = x;
        pos_y = y;
        sprite.setTexture(texture);
        sprite.setPosition(pos_x, pos_y);

        text.setFillColor(szary);
        text.setCharacterSize(35);
        text.setFont(font);
        text.setString(napis);
        text.setPosition(pos_x + 30, pos_y + 30);

        button.setSize(sf::Vector2f(320, 100));
        button.setPosition(pos_x, pos_y);
    }
    if (type == "small")
    {
        if (!texture.loadFromFile("graphic/button_small.png")) {
            std::cout << "Nie zaladowano grafiki\n";
        }
        pos_x = x;
        pos_y = y;
        sprite.setTexture(texture);
        sprite.setPosition(pos_x, pos_y);

        text.setFillColor(szary);
        text.setCharacterSize(12);
        text.setFont(font);
        text.setString(napis);
        text.setPosition(pos_x + 13, pos_y + 4);

        button.setSize(sf::Vector2f(60, 23));
        button.setPosition(pos_x, pos_y);
    }

}


void BUTTON::draw(sf::RenderWindow& window) {
    window.draw(button);
    window.draw(sprite);
    window.draw(text);
}

