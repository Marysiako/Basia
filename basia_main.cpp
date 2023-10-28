#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>
#include <iostream>
#include <sstream>

#include "BUTTON.h"
const int W = 1000;
const int H = 600;
int screen_number = 0;

int main()
{
    //BACKGROUND
    sf::RenderWindow window(sf::VideoMode(W,H), "Basia");
    window.setFramerateLimit(60);
    sf::Texture background_texture;
    background_texture.loadFromFile("graphic/background.png");
    sf::Sprite background_sprite(background_texture);

    // BUTTONS (CLASS - BUTTON.H, BUTTON.CPP)
    BUTTON tuner_button("tuner",180, 150);
    BUTTON tabcreator_button("tab creator", 500, 150);
    BUTTON metronome_button("metronome", 180, 300);
    BUTTON effects_button("effects", 500, 300);
    BUTTON games_buton("games", 180, 450);
    BUTTON exit_button("exit", 500, 450);

    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            //uzytkownik kliknal zamkniecie okna
            if (event.type == sf::Event::Closed)
                window.close();
             //zamknij okno po wcisnieciu escape
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    //sf::FloatRect tuner_button = tuner_button.getGlobalBounds();
                    sf::FloatRect tuner= tuner_button.getSpriteGlobalBounds();
                    if(tuner.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                    {
                        screen_number = 1; //1 to okno tunera
                    }
                }
            }

        }

        //okno menu
        if (screen_number == 0)
        {
        //tekst.setString(std::__cxx11::to_string(punkty));

            //rysowanie
            window.clear();
            window.draw(background_sprite);

            //DRAWING BUTTONS
            tuner_button.draw(window);
            tabcreator_button.draw(window);
            metronome_button.draw(window);
            effects_button.draw(window);
            games_buton.draw(window);
            exit_button.draw(window);
            window.display();
        }
        if (screen_number == 1)
        {
            //rysowanie
            window.clear();
            window.draw(background_sprite);

            window.display();

        }
    }

    return 0;
}
