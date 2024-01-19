#include "FRET.h"
#include <string>
#include<SFML/System.hpp>
#include <SFML/Graphics.hpp>

FRET::FRET(int x_pos, int y_pos, int x_val, int y_val, int fret_id, std::string fret_sound) {
    //texture.loadFromFile("graphic/fretop0.png");    //zrobic ta grafike
    //sprite.setTexture(texture);
    sprite.setPosition(x_pos, y_pos);
    sprite.setTextureRect(sf::IntRect(0, 0, x_val, y_val));
    sprite.setColor(sf::Color::Transparent);
    //sprite.setColor(sf::Color(255, 255, 255, 128));
    name = fret_sound;

}
void FRET::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}
/*
void FRET::change_texture()
{
    if (opacity == 1)
    {
        //sf::Texture teks;
        //texture.loadFromFile("graphic/fretop1.png");    //zrobic ta grafike
        //sprite.setTexture(texture);
        sprite.setColor(sf::Color{ 0, 0, 0, 255 });
        
    }
    if (opacity == 0)
    {
        //texture.create(0, 0);   //tworze pusta teksture
        //texture.loadFromFile("graphic/fretop0.png");    //zrobic ta grafike
        sprite.setColor(sf::Color::Transparent);
        
    }
}
*/
