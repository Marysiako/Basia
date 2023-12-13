#include "TEXT.h"
#include <string>
#include<SFML/System.hpp>
#include <SFML/Graphics.hpp>

TEXT::TEXT(std::string text_string, int x_pos, int y_pos, int size, std::string collor) {

    sf::Font font;
    font.loadFromFile("minecraft_font.ttf");
    sf::Color szary = sf::Color(105, 105, 105);

    text.setCharacterSize(size);
    text.setFont(font);
    text.setString(text_string);
    text.setPosition(x_pos, y_pos);
    if (collor == "gray")
    {
        text.setFillColor(szary);
    }

}
void TEXT::draw(sf::RenderWindow& window)
{
    window.draw(text);
}

void TEXT::settext(std::string new_text_string)
{
    text.setString(new_text_string);
}