#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>

#include <portaudio.h>

#include <iostream>
#include <sstream>
#include <string>

#include "BUTTON.h"
#include "functions.h"
const int W = 1000;
const int H = 600;
int screen_number = 0; // 0-MENU, 1-TUNER, 2-TAB CREATOR, 3-METRONOME, 4-EFFECTS, 5-GAMES
// DO TUNER
double frequency = 1;

// DO METRONOM
int tempo = 120; // Tempo (bpm)
int beatsPerMeasure = 4; // Liczba taktów w metrum, uderzenia na miare
int note = 4;       // 2 - cwierc nuta      4 - pol nuta    8- cala nuta 16 -
int beatcounter = 4;    // Ustawiam jako liczbe taktow w metrum

bool metronome_power = 1;
std::thread metronomeThread;

// Inicjalizacja dzwiekow do metronomu
void MetronomeThread() {
    sf::SoundBuffer buffer1;
    if (!buffer1.loadFromFile("sound/click.wav")) {
     std::cout << "Nie można załadować pliku audio click.wav." << std::endl;
    }

    sf::SoundBuffer buffer2;
    if (!buffer2.loadFromFile("sound/metronome.wav")) {
    std::cout << "Nie można załadować pliku audio metronome.wav." << std::endl;
    }

    sf::Sound metronome_click;
    metronome_click.setBuffer(buffer1);

    sf::Sound metronome_firstclick;
    metronome_firstclick.setBuffer(buffer2);

    while (metronome_power) {
        // Symulacja działania metronomu
        std::this_thread::sleep_for(std::chrono::milliseconds(60000 / tempo)); //dziele minute na liczbe uderzen
        std::cout << "Click" << std::endl;
        if(beatcounter >= beatsPerMeasure) // np 4 == 4
        {
            metronome_firstclick.play();
            beatcounter = 1; // licze od nowa
        }
        else if (beatcounter<beatsPerMeasure)
        {
            metronome_click.play();
            beatcounter++;
        }
    }
}

int main()
{
    //WINDOW
    sf::RenderWindow window(sf::VideoMode(W,H), "Basia");
    window.setFramerateLimit(60);
    sf::Font font;
    font.loadFromFile("minecraft_font.ttf");

    //BACKGROUND
    sf::Texture background_texture;
    background_texture.loadFromFile("graphic/background.png");
    sf::Sprite background_sprite(background_texture);

    //BACKGROUND COMING SOON
    sf::Texture background_cs_texture;
    background_cs_texture.loadFromFile("graphic/background_cs.png");
    sf::Sprite background_cs_sprite(background_cs_texture);

    // BUTTONS (CLASS - BUTTON.H, BUTTON.CPP)
    BUTTON tuner_button("tuner",160, 150, "big");
    BUTTON tabcreator_button("tab creator", 520, 150, "big");
    BUTTON metronome_button("metronome", 160, 300, "big");
    BUTTON effects_button("effects", 520, 300, "big");
    BUTTON games_buton("games", 160, 450, "big");
    BUTTON exit_button("exit", 520, 450, "big");
    BUTTON back_button("BACK", 930, 550, "small");
    // METRONOME BUTTON
    BUTTON metronome_beats_plus("+", 250 ,160, "small");
    BUTTON metronome_beats_minus("-", 250 ,190, "small");
    BUTTON metronome_notes_plus("+", 650 ,160, "small");
    BUTTON metronome_notes_minus("-", 650 ,190, "small");
    BUTTON bpm_minus_button("-", 250, 320, "small");
    BUTTON bpm_plus_button("+", 650, 320, "small");




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

    // METRONOME

    sf::Text metronome_top_text;
    sf::Text metronome_bottom_text;
    sf::Text bpm_text;

    sf::RectangleShape slider(sf::Vector2f(300, 5)); // Pasek suwaka
    slider.setPosition(345, 400);
    slider.setFillColor(sf::Color::Black);

    sf::CircleShape slider_circle(10); // Kółko suwaka
    slider_circle.setFillColor(sf::Color::White);
    slider_circle.setPosition(495, 395); // Pozycja początkową kółka


    // Przykład ustawienia czcionki i pozycji:
    metronome_top_text.setFont(font); // Zamiast "yourFont" podaj własną czcionkę.
    metronome_bottom_text.setFont(font); // Zamiast "yourFont" podaj własną czcionkę.
    metronome_top_text.setPosition(380, 140); // Ustaw pozycję "x" i "y".
    metronome_bottom_text.setPosition(530, 140); // Ustaw pozycję "x" i "y".
    metronome_top_text.setCharacterSize(80);
    metronome_bottom_text.setCharacterSize(80);
    bpm_text.setFont(font);
    bpm_text.setPosition(390, 280);
    bpm_text.setCharacterSize(80);

    while (window.isOpen())
    {
        // Pobieram ciagle czestotliwosc
        /*
        */
               // NIE DZIALA MIKROFON
        frequency = GetFrequencyFromMicrophone();
        std::cout << "czestotliwosc: " << frequency << "\n";
        /*
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
            // EVENTS MENU
            if (screen_number == 0)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        // menu tuner
                        sf::FloatRect tuner = tuner_button.getSpriteGlobalBounds();
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
                            metronome_power = true;
                        }
                        //menu effects
                        sf::FloatRect effects = effects_button.getSpriteGlobalBounds();
                        if(effects.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 4;
                        }
                        //menu games
                        sf::FloatRect games = games_buton.getSpriteGlobalBounds();
                        if(games.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 5;
                        }
                        //exit button
                        sf::FloatRect exitt = exit_button.getSpriteGlobalBounds();
                        if(exitt.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            window.close();
                        }
                    }
                }
            }
            // TUNER
            if(screen_number == 1)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect backk = back_button.getSpriteGlobalBounds();
                        if(backk.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 0;
                        }
                    }
                }
            }
            // TAB CREATOR
            if(screen_number == 2)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect backk = back_button.getSpriteGlobalBounds();
                        if(backk.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 0;
                        }
                    }
                }
            }
            // METRONOME
            if(screen_number == 3)
            {
                // SUWAK
                if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                // Jeśli przeciągamy myszką, przemieszczamy kółko suwaka i aktualizujemy tempo
                    if(event.mouseMove.x >(346) && event.mouseMove.x <(644))
                    {
                        slider_circle.setPosition(event.mouseMove.x, slider_circle.getPosition().y);
                        tempo = 28+ (slider_circle.getPosition().x - 346) / 300 * 600;
                    }
                }

                //RESZTA PRZYCISKOW
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect backk = back_button.getSpriteGlobalBounds();
                        if(backk.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 0;
                            //wylacz
                            metronome_power = false;
                            if (metronomeThread.joinable())
                            {
                                metronomeThread.join();
                            }
                        }
                        sf::FloatRect mbp = metronome_beats_plus.getSpriteGlobalBounds();
                        if(mbp.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            if(beatsPerMeasure<8)
                            {
                                beatsPerMeasure+=1;
                            }
                        }
                        sf::FloatRect mbm = metronome_beats_minus.getSpriteGlobalBounds();
                        if(mbm.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            if(beatsPerMeasure>1)
                            {
                                beatsPerMeasure-=1;
                            }
                        }
                        sf::FloatRect mnp = metronome_notes_plus.getSpriteGlobalBounds();
                        if(mnp.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            if(note<16)
                            {
                                note=note*2;
                            }
                        }
                        sf::FloatRect mnm = metronome_notes_minus.getSpriteGlobalBounds();
                        if(mnm.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            if(note>1)
                            {
                                note=note/2;
                            }
                        }
                        sf::FloatRect mtp = bpm_plus_button.getSpriteGlobalBounds();
                        if(mtp.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            if(tempo>1)
                            {
                                tempo += 1;
                            }
                        }
                        sf::FloatRect mtm = bpm_minus_button.getSpriteGlobalBounds();
                        if(mtm.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            if(tempo<300)
                            {
                                tempo -= 1;
                            }
                        }

                    }
                }
            }
            // EFFECTS
            if(screen_number == 4)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect backk = back_button.getSpriteGlobalBounds();
                        if(backk.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 0;
                        }
                    }
                }
            }
            // TGAMES
            if(screen_number == 5)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect backk = back_button.getSpriteGlobalBounds();
                        if(backk.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 0;
                        }
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
            back_button.draw(window);
            window.display();
        }
        // TAB CREATOR
        if (screen_number == 2)
        {
            //drawing
            window.clear();
            window.draw(background_cs_sprite);
            back_button.draw(window);
            window.display();
        }
        // METRONOME
        if (screen_number == 3)
        {
             // Konwertowanie beatsPerMeasure i note na stringi
            std::string beatsPerMeasureStr = std::to_string(beatsPerMeasure)+" /";
            std::string noteStr = std::to_string(note);
            std::string bpmstr = std::to_string(tempo);

            metronome_top_text.setString(beatsPerMeasureStr);
            metronome_bottom_text.setString(noteStr);
            bpm_text.setString(bpmstr);
            if (metronome_power)
            {
                if (!metronomeThread.joinable())
                    {
                        metronomeThread = std::thread(MetronomeThread);
                    }
            }
            else
            {
                //CLOSE METRONOME
                if (metronomeThread.joinable())
                {
                    metronomeThread.join();
                }
            }

            // RYSOWANIE
            window.clear();
            window.draw(background_sprite);
            window.draw(metronome_top_text);
            window.draw(metronome_bottom_text);
            metronome_beats_plus.draw(window);
            metronome_beats_minus.draw(window);
            metronome_notes_plus.draw(window);
            metronome_notes_minus.draw(window);
            bpm_plus_button.draw(window);
            bpm_minus_button.draw(window);
            window.draw(bpm_text);
            window.draw(slider);
            window.draw(slider_circle);
            back_button.draw(window);

            window.display();
        }
        // EFFECTS
        if (screen_number == 4)
        {
            //drawing
            window.clear();
            window.draw(background_cs_sprite);
            back_button.draw(window);
            window.display();
        }

        // GAMES
        if (screen_number == 5)
        {
            //drawing
            window.clear();
            window.draw(background_cs_sprite);
            back_button.draw(window);
            window.display();
        }

    }

    return 0;
}
