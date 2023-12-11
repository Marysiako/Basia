#include <SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <string>
#include <iostream>
#include "TEXTURE.h"

TEXTURE::TEXTURE(std::string link, int x, int y) {
    pos_x = x;
    pos_y = y;
    texture.loadFromFile(link);
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
}

void TEXTURE::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}