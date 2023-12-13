#pragma once

#include <string>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>

class TEXT {
public:
    TEXT(std::string text, int x_pos, int y_pos, int size, std::string collor);
    sf::Text text;
    void draw(sf::RenderWindow& window);
    void settext(std::string new_text_string);
private:

};