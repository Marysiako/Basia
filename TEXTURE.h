#ifndef TEXTURE_H
#define TEXTURE_H
#include <SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <string>
#include <iostream>

class TEXTURE {
public:
    TEXTURE(std::string link, int x, int y); 
    void draw(sf::RenderWindow& window);
    int pos_x;
    int pos_y;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::FloatRect getSpriteGlobalBounds() {
        return sprite.getGlobalBounds();
    }
private:
};
#endif