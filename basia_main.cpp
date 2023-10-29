#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>
/*
#include <portaudio.h>
*/
#include <iostream>
#include <sstream>

#include "BUTTON.h"
#include "functions.h"
const int W = 1000;
const int H = 600;
int screen_number = 3; // 0-MENU, 1-TUNER, 2-TAB CREATOR, 3-EFFECTS, 4-GAMES
// DO TUNER
double frequency = 1;

// DO METRONOM
int tempo = 120; // Tempo (bpm)
int beatsPerMeasure = 4; // Liczba taktów w metrum
bool metronome_power = 1;
std::thread metronomeThread;

void MetronomeThread() {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sound/click.wav")) {
    // Obsługa błędu, jeśli nie można załadować pliku audio.
    // Możesz wyświetlić komunikat o błędzie lub podjąć odpowiednie kroki.
    }

    sf::Sound metronome_click;
    metronome_click.setBuffer(buffer);

    while (metronome_power) {
        // Symulacja działania metronomu
        std::this_thread::sleep_for(std::chrono::milliseconds(60000 / tempo));
        std::cout << "Click" << std::endl;
        metronome_click.play();
    }
}

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

    // OTHER TEXTURES
        //TUNER
        int line_x = W/2;
        int line_y = H/2-20;
    sf::Texture tuner_texture;
    tuner_texture.loadFromFile("graphic/tuner.png");
    sf::Sprite tuner_sprite(tuner_texture);
    tuner_sprite.setPosition(W/2-307, 150);

    sf::Texture line_texture;
    line_texture.loadFromFile("graphic/line.png");
    sf::Sprite line_sprite(line_texture);
    line_sprite.setPosition(line_x, line_y);

    while (window.isOpen())
    {
        // Pobieram ciagle czestotliwosc
        /*
                NIE DZIALA MIKROFON
        frequency = GetFrequencyFromMicrophone();

        */
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
                    // menu tuner
                    sf::FloatRect tuner= tuner_button.getSpriteGlobalBounds();
                    if(tuner.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                    {
                        screen_number = 1; //1 to okno tunera
                    }
                    // menu tab creator
                    sf::FloatRect tabcr= tabcreator_button.getSpriteGlobalBounds();
                    if(tabcr.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                    {
                        screen_number = 2;
                    }
                    //menu metronome
                    sf::FloatRect metronome= metronome_button.getSpriteGlobalBounds();
                    if(metronome.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                    {
                        screen_number = 3;
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
        // TUNER
        if (screen_number == 1)
        {
            //drawing
            window.clear();
            window.draw(background_sprite);
            window.draw(tuner_sprite);
            window.draw(line_sprite);

            window.display();
        }
        // TAB CREATOR
        if (screen_number == 2)
        {
            //drawing
            window.clear();
            window.draw(background_sprite);

            window.display();
        }
        // METRONOME
        if (screen_number == 3)
        {
            if (metronome_power)
            {
                if (!metronomeThread.joinable())
                    {
                        metronomeThread = std::thread(MetronomeThread);
                    }
            }
            else
            {
                if (metronomeThread.joinable())
                {
                    metronomeThread.join();
                }
            }

            // RYSOWANIE
            window.clear();
            window.draw(background_sprite);

            window.display();
        }
    }
    if (metronomeThread.joinable()) {
        metronome_power = false;
        metronomeThread.join();
    }

    return 0;
}
