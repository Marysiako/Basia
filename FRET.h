#ifndef FRET_H
#define FRET_H

#include <string>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>

class FRET {
public:
    FRET(int x_pos, int y_pos, int x_val, int y_val, int fret_id, std::string fret_sound);       // pozycja x,y rozmiar x, y
    sf::Texture texture;
    sf::Sprite sprite;
    void draw(sf::RenderWindow& window);
    void change_texture();
    int opacity = 1;
private:

};
#endif