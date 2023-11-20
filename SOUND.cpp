#include "SOUND.h"
#include <portaudio.h>
#include <iostream>
#include <sstream>
#include <string>
#include<SFML/System.hpp>
#include <SFML/Audio.hpp>

SOUND::SOUND(std::string filename) {

    if (!buffer.loadFromFile(filename)) {
     std::cout << "Nie można załadować pliku audio "+filename << std::endl;
    }else {
     std::cout << "Zaladowano "+filename << std::endl;
    }

    sound.setBuffer(buffer);
}

void SOUND::play_sound()
{
    sound.play();
    std::cout << "gram\n";
}
