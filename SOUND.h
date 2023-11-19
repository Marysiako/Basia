#pragma once

#include <string>
#include<SFML/System.hpp>
#include <SFML/Audio.hpp>

class SOUND {
public:
    SOUND(std::string filename);
    sf::SoundBuffer buffer;
    sf::Sound sound;
    void play_sound();
private:

};

