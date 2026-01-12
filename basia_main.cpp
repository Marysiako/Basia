//#include "AUDIO.h"
/*

TAK DZIALA:
 g++ basia_main.cpp BUTTON.cpp functions.cpp SOUND.cpp TEXTURE.cpp FRET.cpp     -o sfml-app     -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio     -lportaudio -lfftw3f


*/
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>
#include <fstream>

#include <portaudio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <mutex>
#include <fftw3.h>
#include <cmath>
#include <iomanip>

#include <atomic>

#include "BUTTON.h"
#include "functions.h"
#include "SOUND.h"
#include "TEXTURE.h"
#include "TEXT.h"
#include "FRET.h"

//DO ODCZYTU CZESTOTLIWOSCI
#define SAMPLE_RATE 44100   // czestotliwosc probkowania
#define FRAMES_PER_BUFFER 256 // liczba probek na jeden callback
#define BUFFER_SIZE 32768   // rozmiar bufora - nim wiekszy tym dokladniej

const int W = 1000;
const int H = 600;
int screen_number = 0; // 0-MENU, 1-TUNER, 2-TAB CREATOR, 3-METRONOME, 4-EFFECTS, 5-GAMES
int game = 0;
// DO TUNER
double frequency = 1;

//DO BOOK
int bookPage = 1;

// DO METRONOM------------------------------------------------------------------------------
int tempo = 120; // Tempo (bpm)
int beatsPerMeasure = 4; // Liczba taktów w metrum, uderzenia na miare
int note = 4;       // 2 - cwierc nuta      4 - pol nuta    8- cala nuta 16 -
int beatcounter = 4;    // Ustawiam jako liczbe taktow w metrum

//DO GAMES1---------------------------------------------------------------------------------
int randomIndex_sound = 1;
std::string random_sound_uget_string = "bass_sounds/e3.wav";
//SOUND random_sound_uget(random_sound_uget_string);
sf::SoundBuffer random_sound_buffer;

std::string choosed_sound = "i";
std::string guessedsound_string = "Do you know the sound?";

//DO GAME 2 ---------------------------------------------------------------------------------
bool show_fr = 0;
std::string wylosowany_dzwiek = "E";
//sf::Text wylosowany_dzwiek;

//DO METRONOMU -----------------------------------------------------------------------------
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
        //std::cout << "Click" << std::endl;
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
//DO TUNER I TAB CREATOR -------------------------------------------------------------------
//DRUGI WATEK ODCZYTUJACY CZESTOTLIWOSC
std::atomic<float> currentFreq{0.0f};
std::atomic<float> currentVolume{0.0f};
std::atomic<float> currentSF{0.0f};
std::atomic<bool> running{true};

sf::SoundBuffer E_buffer, A_buffer, D_buffer, G_buffer;
sf::Sound E_sound, A_sound, D_sound, G_sound;

//TAB CREATOR---------------------------------------
struct Note {
    char stringName;
    float frequency;
    int fret;
};
std::vector<Note> notes = {
    // E string (E1 = 41.20 Hz)
    {'E', 41.20, 0}, {'E', 43.65, 1}, {'E', 46.25, 2}, {'E', 49.00, 3}, {'E', 51.91, 4},
    {'E', 55.00, 5}, {'E', 58.27, 6}, {'E', 61.74, 7}, {'E', 65.41, 8}, {'E', 69.30, 9},
    {'E', 73.42, 10}, {'E', 77.78, 11}, {'E', 82.41, 12}, {'E', 87.31, 13}, {'E', 92.50, 14},
    {'E', 98.00, 15}, {'E', 103.83, 16}, {'E', 110.00, 17}, {'E', 116.54, 18}, {'E', 123.47, 19}, 

    // A string (A1 = 55.00 Hz)
    {'A', 55.00, 0}, {'A', 58.27, 1}, {'A', 61.74, 2}, {'A', 65.41, 3}, {'A', 69.30, 4},
    {'A', 73.42, 5}, {'A', 77.78, 6}, {'A', 82.41, 7}, {'A', 87.31, 8}, {'A', 92.50, 9},
    {'A', 98.00, 10}, {'A', 103.83, 11}, {'A', 110.00, 12}, {'A', 116.54, 13}, {'A', 123.47, 14},
    {'A', 130.81, 15}, {'A', 138.59, 16}, {'A', 146.83, 17}, {'A', 155.56, 18}, {'A', 164.81, 19},

    // D string (D2 = 73.42 Hz)
    {'D', 73.42, 0}, {'D', 77.78, 1}, {'D', 82.41, 2}, {'D', 87.31, 3}, {'D', 92.50, 4},
    {'D', 98.00, 5}, {'D', 103.83, 6}, {'D', 110.00, 7}, {'D', 116.54, 8}, {'D', 123.47, 9},
    {'D', 130.81, 10}, {'D', 138.59, 11}, {'D', 146.83, 12}, {'D', 155.56, 13}, {'D', 164.81, 14},
    {'D', 174.61, 15}, {'D', 185.00, 16}, {'D', 196.00, 17}, {'D', 207.65, 18}, {'D', 220.00, 19},

    // G string (G2 = 98.00 Hz)
    {'G', 98.00, 0}, {'G', 103.83, 1}, {'G', 110.00, 2}, {'G', 116.54, 3}, {'G', 123.47, 4},
    {'G', 130.81, 5}, {'G', 138.59, 6}, {'G', 146.83, 7}, {'G', 155.56, 8}, {'G', 164.81, 9},
    {'G', 174.61, 10}, {'G', 185.00, 11}, {'G', 196.00, 12}, {'G', 207.65, 13}, {'G', 220.00, 14},
    {'G', 233.08, 15}, {'G', 246.94, 16}, {'G', 261.63, 17}, {'G', 277.18, 18}, {'G', 293.66, 19}

};
struct TabFrame {
    std::string E = "E|-";
    std::string A = "A|-";
    std::string D = "D|-";
    std::string G = "G|-";
};
TabFrame myTabs; //Struktura przechowujaca stringi dla wszystkich strun
// FUnkcja znajdujaca pasujace progi na gryfie do zagranej czestotliwosci zwraca vektor wszytskich pasujacych dzwiekow
std::vector<Note>  findMatchingNotes(float freq) {
    std::vector<Note> matchingNotes;
    float minDiff = 1.0f; //minimalnie jest okolo 2.4Hz roznicy miedzy dziwkami wiec troche mniej niz polowe daje jakby nie stroil idealnie

    for (const auto& note : notes) {
        float diff = std::abs(note.frequency - freq);
        if (diff < minDiff) {
            matchingNotes.push_back(note);
        }
    }
    return matchingNotes;
}

static std::vector<float> volumeHistory;  //historia Spectral FLux energi
static std::vector<float>  history; // historia SF do liczenia progu adaptacyjnego
static std::vector<float> frequencyHistory; //historia odczytow czestotliwosci
static std::vector<float> dtHistory; // historia czasow odczytow danych



//Odczytane na zywo dziweki
std::vector<std::string> splitLines(const std::string& s)
{
    std::vector<std::string> lines;
    std::stringstream ss(s);
    std::string line;

    while (std::getline(ss, line)) {
        if (!line.empty())          // pomijamy puste linie
            lines.push_back(line);
    }
    return lines;
}
void saveTabToFile(const TabFrame& tab, const std::string& filename)
{
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Nie można otworzyć pliku do zapisu!\n";
        return;
    }

    auto GLines = splitLines(tab.G);
    auto DLines = splitLines(tab.D);
    auto ALines = splitLines(tab.A);
    auto ELines = splitLines(tab.E);

    size_t blocks = std::max({
        GLines.size(),
        DLines.size(),
        ALines.size(),
        ELines.size()
    });

    for (size_t i = 0; i < blocks; ++i)
    {
        outFile << (i < GLines.size() ? GLines[i] : "") << "\n";
        outFile << (i < DLines.size() ? DLines[i] : "") << "\n";
        outFile << (i < ALines.size() ? ALines[i] : "") << "\n";
        outFile << (i < ELines.size() ? ELines[i] : "") << "\n\n";
    }

    outFile.close();
}

void saveHistoryToFile(){
    std::ofstream outFile("history.txt"); 
    if (!outFile) {
        std::cerr << "Nie można otworzyć pliku do zapisu!\n";
        return;
    }
    size_t size = std::min({  // biore najmniejsza dlugosc chociaz powinny byc takie same
        dtHistory.size(), 
        volumeHistory.size(), 
        frequencyHistory.size() 
    });

    for (size_t i = 0; i < size; ++i)       // dt vol freq
    {
        outFile << dtHistory[i] << " "
                << volumeHistory[i] << " "
                << frequencyHistory[i] << "\n";
    }

    outFile.close();
    
}
int main()
{
    if (!InitAudio()) {
        std::cerr << "Błąd inicjalizacji audio!\n";
        return -1;
    }
    //WINDOW
    sf::RenderWindow window(sf::VideoMode(W,H), "Basia");
    window.setFramerateLimit(60);
    sf::Font font;
    font.loadFromFile("minecraft_font.ttf");
    sf::Color szary = sf::Color(105, 105, 105);

    TEXTURE background_sprite("graphic/background.png", 0, 0);
    TEXTURE background_cs_sprite("graphic/background_cs.png", 0, 0);    //BACKGROUND COMING SOON
    TEXTURE background_clear_sprite("graphic/background_clear.png", 0, 0);
    TEXTURE tuner_sprite("graphic/tuner.png", W/2-307, 150);        //TEXTURE OF TUNER GRAPHIC
    TEXTURE metronome_tabs_sprite("graphic/background_training.png", 0, 0);     //Tabs graphic
    TEXTURE fretboard_clear_sprite("graphic/fretboard.png", 0, 200);
    TEXTURE freatboard_filled_sprite("graphic/fretboard_filled.png", 0, 200);
    TEXTURE background_bookChords_sprite("graphic/background_chords.png",0,0); //1
    TEXTURE background_bookScales_sprite("graphic/background_scales.png",0,0); //2
    TEXTURE background_bookIntervals_sprite("graphic/background_intervals.png",0,0); //3
    TEXTURE background_bookShapes_sprite("graphic/background_patterns.png",0,0); //4
    TEXTURE background_bookCircle_sprite("graphic/background_circle.png",0,0); //5
    TEXTURE background_bookRules_sprite("graphic/background_rules.png",0,0); //6
    TEXTURE background_bookTabs_sprite("graphic/background_tabs.png",0,0); //7

    if (!random_sound_buffer.loadFromFile(random_sound_uget_string)) {
     std::cout << "Nie można załadować pliku audio "+random_sound_uget_string << std::endl;
    }
    sf::Sound random_sound_sound;
    random_sound_sound.setBuffer(random_sound_buffer);
    
    //TUNER
    E_buffer.loadFromFile("bass_sounds/e1.wav"); E_sound.setBuffer(E_buffer);
    A_buffer.loadFromFile("bass_sounds/a1.wav"); A_sound.setBuffer(A_buffer);
    D_buffer.loadFromFile("bass_sounds/d1.wav"); D_sound.setBuffer(D_buffer);
    G_buffer.loadFromFile("bass_sounds/g2.wav"); G_sound.setBuffer(G_buffer);

    // BUTTONS (CLASS - BUTTON.H, BUTTON.CPP)
    BUTTON tuner_button("tuner",160, 150, "big");
    BUTTON tabcreator_button("tab creator", 520, 150, "big");
    BUTTON metronome_button("metronome", 160, 300, "big");
    BUTTON effects_button("book", 520, 300, "big"); //Zmieniam efekty na ksiazeczke z teoria
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
    BUTTON tabs_button("training tabs", 320,450, "big");
    BUTTON back_metronome_button("metronome", 930,550, "small");
    //GAMES BUTTON
    BUTTON game1_button("Guess the sound", 300, 170, "bigger");
    BUTTON game2_button("Guess the fret", 300, 370, "bigger");
    BUTTON gamesingame_button("Games", 830, 550, "small"); 
    BUTTON random_sound_button("Get the sound", 270, 150, "bigger");
    BUTTON fretboard_fill_button("Show", 750, 550, "small");

    std::string a_text = "e";
    std::string b_text = "a";
    std::string c_text = "d";
    BUTTON abc_a_button(a_text, 200, 350, "medium");
    BUTTON abc_b_button(b_text, 400, 350, "medium");
    BUTTON abc_c_button(c_text, 600, 350, "medium");

    //BOOK BUTTON
    BUTTON bookChordsButton("chords", 50, 50, "biggersmall");
    BUTTON bookScalesButton("scales", 180, 50, "biggersmall");
    BUTTON bookIntervalsButton("intervals", 310, 50, "biggersmall");
    BUTTON bookShapesButton("shapes", 440, 50, "biggersmall");
    BUTTON bookCircleButton("circle", 570, 50, "biggersmall");
    BUTTON bookRulesButton("rules", 700, 50, "biggersmall");
    BUTTON bookTabsButton("tabs", 830, 50, "biggersmall");

    //TAB CREATOR
    BUTTON tabCreatorStartButton("start", 100, 50, "small");
    BUTTON tabCreatorStopButton("stop", 200, 50, "small");
    BUTTON tabCreatorSaveButton("save", 700, 50, "small");
    BUTTON tabCreatorResetButton("reset", 800, 50, "small");
    bool tabCreatorRunning = false;
    sf::Text tabCreatorEText("E", font, 10);
    tabCreatorEText.setPosition(10, 175);
    sf::Text tabCreatorAText("A", font, 10);
    tabCreatorAText.setPosition(10, 150);
    sf::Text tabCreatorDText("D", font, 10);
    tabCreatorDText.setPosition(10, 125);
    sf::Text tabCreatorGText("G", font, 10);
    tabCreatorGText.setPosition(10, 100);
    

    //SOUNDS
    std::string sound_names[]  = {"e","f","fs","g","gs","a","as","b","c","cs","d","ds"};

    // OTHER TEXTURES
    //TUNER ---------------------------------------------------------------------
    double line_x = W/2;       //LINIA DO TUNERA, WSKAZOWKA
    double line_y = H/2-20;
    sf::Texture line_texture;           // Tej tekstury nie zmieniam na klas ebo pozniej zmieniam pozycje
    line_texture.loadFromFile("graphic/line.png");
    sf::Sprite line_sprite(line_texture);
    line_sprite.setPosition(line_x, line_y);
    sf::Texture play_sound_texture; 
    play_sound_texture.loadFromFile("graphic/play_sound.png");
    sf::Sprite play_E(play_sound_texture);
    sf::Sprite play_A(play_sound_texture);
    sf::Sprite play_D(play_sound_texture);
    sf::Sprite play_G(play_sound_texture);
    play_G.setPosition(780, 220);
    play_D.setPosition(780, 280);
    play_A.setPosition(780, 345);
    play_E.setPosition(780, 410);

    //W TUNER
    sf::Text freqText("", font, 20);
    freqText.setFillColor(sf::Color::Red);
    freqText.setPosition(10, 10);

    sf::Text freqEADGText("", font, 20);
    freqEADGText.setFillColor(sf::Color::Red);
    freqEADGText.setPosition(10, 550);
    freqEADGText.setString("E = 41.20Hz, A = 55Hz, D = 73.42H, G = 98Hz");

    //---------------------------------------------------------------------
    // METRONOM
    sf::RectangleShape slider(sf::Vector2f(300, 5));    // Pasek suwaka
    slider.setPosition(345, 400);
    slider.setFillColor(sf::Color::Black);

    sf::CircleShape slider_circle(10);      // Kolko suwaka
    slider_circle.setFillColor(sf::Color::White);
    slider_circle.setPosition(495, 395); // Pozycja poczatkowa kolka


    // czcionki i pozycje
    //Metronom
    //TEXT metronome_top_text("4", 380, 140, 80, "szary");
    sf::Text metronome_top_text;
    sf::Text metronome_bottom_text;
    sf::Text bpm_text;

    metronome_top_text.setFont(font);
    metronome_bottom_text.setFont(font);
    metronome_top_text.setPosition(380, 140);
    metronome_bottom_text.setPosition(530, 140);
    metronome_top_text.setCharacterSize(80);
    metronome_bottom_text.setCharacterSize(80);
    metronome_top_text.setFillColor(szary);
    metronome_bottom_text.setFillColor(szary);
    bpm_text.setFillColor(szary);
    bpm_text.setFont(font);
    bpm_text.setPosition(390, 280);
    bpm_text.setCharacterSize(80);

    //Games
    sf::Text games_text;
    games_text.setFont(font);
    games_text.setString("Games");
    games_text.setCharacterSize(80);
    games_text.setPosition(370, 20);
    games_text.setFillColor(szary);
    
    sf::Text guessedsound_text;
    guessedsound_text.setFont(font);
    guessedsound_text.setString(guessedsound_string);
    guessedsound_text.setCharacterSize(40);
    guessedsound_text.setPosition(40, 520);
    guessedsound_text.setFillColor(szary);

     // FRETS FOR GAME 2
    FRET testowy_fret(57, 200, 50, 45, 101, "G");
    FRET tablicaObiektow[] = {
        FRET(57, 200, 50, 45, 101, "G#"),      //G
        FRET(107, 200, 55, 45, 102, "A"),
        FRET(162, 200, 55, 45, 103, "A#"),
        FRET(217, 200, 50, 45, 104, "B"),
        FRET(267, 200, 48, 45, 105, "C"),
        FRET(315, 200, 45, 45, 106, "C#"),
        FRET(360, 200, 40, 45, 107, "D"),
        FRET(400, 200, 40, 45, 108, "D#"),
        FRET(440, 200, 38, 45, 109, "E"),
        FRET(478, 200, 35, 45, 110, "F"), //10
        FRET(513, 200, 30, 45, 111, "F#"),
        FRET(543, 200, 32, 45, 112, "G"),
        FRET(575, 200, 32, 45, 113, "G#"),
        FRET(607, 200, 32, 45, 114, "A"),
        FRET(639, 200, 35, 45, 115, "A#"), //15
        FRET(674, 200, 33, 45, 116, "B"),
        FRET(707, 200, 33, 45, 117, "C"),
        FRET(740, 200, 30, 45, 118, "C#"),
        FRET(770, 200, 30, 45, 119, "D"),
        FRET(800, 200, 35, 45, 120, "D#"), //20
        FRET(835, 200, 34, 45, 121, "E"),
        FRET(869, 200, 30, 45, 122, "F"),
        FRET(899, 200, 32, 45, 123, "F#"),
        FRET(931, 200, 35, 45, 124, "G"), //24

        FRET(57, 245, 50, 40, 201, "D#"),    //D
        FRET(107, 245, 55, 40, 202, "E"),
        FRET(162, 245, 55, 40, 203, "F"),
        FRET(217, 245, 50, 40, 204, "F#"),
        FRET(267, 245, 48, 40, 205, "G"),
        FRET(315, 245, 45, 40, 206, "G#"),
        FRET(360, 245, 40, 40, 207, "A"),
        FRET(400, 245, 40, 40, 208, "A#"),
        FRET(440, 245, 38, 40, 209, "B"),
        FRET(478, 245, 35, 40, 210, "C"), //10
        FRET(513, 245, 30, 40, 211, "C#"),
        FRET(543, 245, 32, 40, 212, "D"),
        FRET(575, 245, 32, 40, 213, "D#"),
        FRET(607, 245, 32, 40, 214, "E"),
        FRET(639, 245, 35, 40, 215, "F"), //15
        FRET(674, 245, 33, 40, 216, "F#"),
        FRET(707, 245, 33, 40, 217, "G"),
        FRET(740, 245, 30, 40, 218, "G#"),
        FRET(770, 245, 30, 40, 219, "A"),
        FRET(800, 245, 35, 40, 220, "A#"), //20
        FRET(835, 245, 34, 40, 221, "B"),
        FRET(869, 245, 30, 40, 222, "C"),
        FRET(899, 245, 32, 40, 223, "C#"),
        FRET(931, 245, 35, 40, 224, "D"), //24

        FRET(57, 285, 50, 40, 301, "A#"), //A
        FRET(107, 285, 55, 40, 302, "B"),
        FRET(162, 285, 55, 40, 303, "C"),
        FRET(217, 285, 50, 40, 304, "C#"),
        FRET(267, 285, 48, 40, 305, "D"),
        FRET(315, 285, 45, 40, 306, "D#"),
        FRET(360, 285, 40, 40, 307, "E"),
        FRET(400, 285, 40, 40, 308, "F"),
        FRET(440, 285, 38, 40, 309, "F#"),
        FRET(478, 285, 35, 40, 310, "G"), //10
        FRET(513, 285, 30, 40, 311, "G#"),
        FRET(543, 285, 32, 40, 312, "A"),
        FRET(575, 285, 32, 40, 313, "A#"),
        FRET(607, 285, 32, 40, 314, "B"),
        FRET(639, 285, 35, 40, 315, "C"), //15
        FRET(674, 285, 33, 40, 316, "C#"),
        FRET(707, 285, 33, 40, 317, "D"),
        FRET(740, 285, 30, 40, 318, "D#"),
        FRET(770, 285, 30, 40, 319, "E"),
        FRET(800, 285, 35, 40, 320, "F"), //20
        FRET(835, 285, 34, 40, 321, "F#"),
        FRET(869, 285, 30, 40, 322, "G"),
        FRET(899, 285, 32, 40, 323, "G#"),
        FRET(931, 285, 35, 40, 324, "A"), //24

        FRET(57, 325, 50, 35, 401, "F"),    //E
        FRET(107, 325, 55, 35, 402, "F#"),
        FRET(162, 325, 55, 35, 403, "G"),
        FRET(217, 325, 50, 35, 404, "G#"),
        FRET(267, 325, 48, 35, 405, "A"),
        FRET(315, 325, 45, 35, 406, "A#"),
        FRET(360, 325, 40, 35, 407, "B"),
        FRET(400, 325, 40, 35, 408, "C"),
        FRET(440, 325, 38, 35, 409, "C#"),
        FRET(478, 325, 35, 35, 410, "D"), //10
        FRET(513, 325, 30, 35, 411, "D#"),
        FRET(543, 325, 32, 35, 412, "E"),
        FRET(575, 325, 32, 35, 413, "F"),
        FRET(607, 325, 32, 35, 414, "F#"),
        FRET(639, 325, 35, 35, 415, "G"), //15
        FRET(674, 325, 33, 35, 416, "G#"),
        FRET(707, 325, 33, 35, 417, "A"),
        FRET(740, 325, 30, 35, 418, "A#"),
        FRET(770, 325, 30, 35, 419, "B"),
        FRET(800, 325, 35, 35, 420, "C"), //20
        FRET(835, 325, 34, 35, 421, "C#"),
        FRET(869, 325, 30, 35, 422, "D"),
        FRET(899, 325, 32, 35, 423, "D#"),
        FRET(931, 325, 35, 35, 424, "E"), //24

    };
    
    std::string tablicaNazw[] = {"E", "F", "F#", "G", "G#", "A", "A#", "B", "C", "C#", "D", "D#"};
    std::string wybrananazwa = "";
    //std::string informacja_zwrotna_g2 = "";
    std::string los_game2 = tablicaNazw[GiveRandomIndex(12)];
    int points_game2 = 0;
    sf::Text text_los_game2;
    text_los_game2.setFont(font);
    text_los_game2.setPosition(50, 50);
    text_los_game2.setCharacterSize(50);
    text_los_game2.setFillColor(szary);
    text_los_game2.setString("Find: "+los_game2);

    sf::Text text_wybrananazwa;
    text_wybrananazwa.setFont(font);
    text_wybrananazwa.setPosition(450, 50);
    text_wybrananazwa.setCharacterSize(50);
    text_wybrananazwa.setFillColor(szary);
    text_wybrananazwa.setString("You chose: "+wybrananazwa);

    sf::Text text_points;
    text_points.setFont(font);
    text_points.setPosition(450, 450);
    text_points.setCharacterSize(50);
    text_points.setFillColor(szary);
    text_points.setString("Points: "+std::to_string(points_game2));



    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            //uzytkownik kliknal zamkniecie okna
            if (event.type == sf::Event::Closed)
            {
                window.close();
                running = false;  // sygnal do watku
                CloseAudio();
                break;
            }
             //zamknij okno po wcisnieciu escape
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
                running = false;  
                CloseAudio();
                break;
            }
                
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
                        sf::FloatRect ple = play_E.getGlobalBounds();
                        if(ple.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            E_sound.play();
                        }
                        sf::FloatRect pla = play_A.getGlobalBounds();
                        if(pla.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            A_sound.play();
                        }
                        sf::FloatRect pld = play_D.getGlobalBounds();
                        if(pld.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            D_sound.play();
                        }
                        sf::FloatRect plg = play_G.getGlobalBounds();
                        if(plg.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            G_sound.play();
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
                            tabCreatorRunning = false;
                        }
                    }
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect s = tabCreatorStartButton.getSpriteGlobalBounds();
                        if(s.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            //myTabs zeruje i zaczyna leciec petla zapisujaca dzwieki
                            myTabs.E = "E|";
                            myTabs.A = "A|";
                            myTabs.D = "D|";
                            myTabs.G = "G|";
                            tabCreatorEText.setString(myTabs.E);
                            tabCreatorAText.setString(myTabs.A);
                            tabCreatorDText.setString(myTabs.D);
                            tabCreatorGText.setString(myTabs.G); 
                            tabCreatorRunning = true;
                        }
                    }
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect s = tabCreatorStopButton.getSpriteGlobalBounds();
                        if(s.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            //przestaje czytac dzwieki
                            tabCreatorRunning = false;
                        }
                    }
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect s = tabCreatorSaveButton.getSpriteGlobalBounds();
                        if(s.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            //zapisuje aktualny stan myTabs i zapisuje do pliku txt
                            tabCreatorRunning = false;
                            saveTabToFile(myTabs, "moja_tabulaturaBasia");
                            saveHistoryToFile();
                            
                        }
                    }
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect s = tabCreatorResetButton.getSpriteGlobalBounds();
                        if(s.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            //zapisuje aktualny stan myTabs i zapisuje do pliku txt
                            tabCreatorRunning = false;
                            myTabs.E = "E|";
                            myTabs.A = "A|";
                            myTabs.D = "D|";
                            myTabs.G = "G|";
                            tabCreatorEText.setString(myTabs.E);
                            tabCreatorAText.setString(myTabs.A);
                            tabCreatorDText.setString(myTabs.D);
                            tabCreatorGText.setString(myTabs.G);    
                            volumeHistory.clear();
                            frequencyHistory.clear();
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
                        sf::FloatRect tab = tabs_button.getSpriteGlobalBounds();
                        if(tab.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 31;
                        }

                    }
                }
            }
            if(screen_number ==31)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect backm = back_metronome_button.getSpriteGlobalBounds();
                        if(backm.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 3;
                        }
                    }
                }
            }
            // EFFECTS (BOOK)
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
                        //chords - 1
                        sf::FloatRect chords = bookChordsButton.getSpriteGlobalBounds();
                        if(chords.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 1;
                        }
                        //scales = 2
                        sf::FloatRect scales = bookScalesButton.getSpriteGlobalBounds();
                        if(scales.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 2;
                        }
                        //intervals = 3
                        sf::FloatRect intervals = bookIntervalsButton.getSpriteGlobalBounds();
                        if(intervals.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 3;
                        }
                        //patterns = 4
                        sf::FloatRect patterns = bookShapesButton.getSpriteGlobalBounds();
                        if(patterns.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 4;
                        }
                        //circle = 5;
                        sf::FloatRect circle = bookCircleButton.getSpriteGlobalBounds();
                        if(circle.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 5;
                        }
                        //rules = 6;
                        sf::FloatRect rules = bookRulesButton.getSpriteGlobalBounds();
                        if(rules.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 6;
                        }
                        //tabs = 7;
                        sf::FloatRect tabs = bookTabsButton.getSpriteGlobalBounds();
                        if(tabs.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            bookPage = 7;
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
                        sf::FloatRect game1_ = game1_button.getSpriteGlobalBounds();
                        if(game1_.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 51;
                            
                        }
                        sf::FloatRect game2_ = game2_button.getSpriteGlobalBounds();
                        if(game2_.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 52;
                        }
                    }
                }
            }
            if(screen_number == 51)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect gamesin = gamesingame_button.getSpriteGlobalBounds();
                        if(gamesin.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 5;
                        }

                        sf::FloatRect getsound = random_sound_button.getSpriteGlobalBounds();
                        if(getsound.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            randomIndex_sound = GiveRandomIndex(11);    //dzwiek
                            //std::cout << sound_names[randomIndex_sound] << std::endl;

                            int randomIndex_exac= GiveRandomIndex(3)+1;      // numer nagrania (kazdy dzwiek ma 3 wersje 1 2 3)
                            
                            random_sound_uget_string = "bass_sounds/"+sound_names[randomIndex_sound]+std::to_string(randomIndex_exac)+".wav";
                            random_sound_buffer.loadFromFile(random_sound_uget_string);
                            random_sound_sound.setBuffer(random_sound_buffer);
                            random_sound_sound.play();

                            int randomIndex_position= GiveRandomIndex(3)+1;   //pozycja a b lgdzie lub c dzie bedzie poprawny dziek  
 
                            if (randomIndex_position == 1)
                            {
                                a_text = sound_names[randomIndex_sound];
                                b_text = sound_names[GiveRandomIndex(11)];
                                c_text = sound_names[GiveRandomIndex(11)];
                                while((b_text == a_text) || (c_text == a_text))
                                {
                                    b_text = sound_names[GiveRandomIndex(11)];
                                    c_text = sound_names[GiveRandomIndex(11)];
                                }
                                abc_a_button.changeText(a_text);
                                abc_b_button.changeText(b_text);
                                abc_c_button.changeText(c_text);
                                
                                //std::cout << random_sound_uget_string;
                                //std::cout << "poz 1\n\n";
                            } 
                            if (randomIndex_position == 2)
                            {   
                                b_text = sound_names[randomIndex_sound];
                                a_text = sound_names[GiveRandomIndex(11)];
                                c_text = sound_names[GiveRandomIndex(11)];
                                while((b_text == a_text) || (c_text == b_text))
                                {
                                    a_text = sound_names[GiveRandomIndex(11)];
                                    c_text = sound_names[GiveRandomIndex(11)];
                                }
                                abc_a_button.changeText(a_text);
                                abc_b_button.changeText(b_text);
                                abc_c_button.changeText(c_text);
                                
                                //std::cout << random_sound_uget_string;
                                //std::cout << "poz 2\n\n";
                            } 
                            if (randomIndex_position == 3)
                            {
                                c_text = sound_names[randomIndex_sound];
                                b_text = sound_names[GiveRandomIndex(11)];
                                a_text = sound_names[GiveRandomIndex(11)];
                                while((b_text == c_text) || (c_text == a_text))
                                {
                                    b_text = sound_names[GiveRandomIndex(11)];
                                    a_text = sound_names[GiveRandomIndex(11)];
                                }
                                abc_a_button.changeText(a_text);
                                abc_b_button.changeText(b_text);
                                abc_c_button.changeText(c_text);
                                
                                //std::cout << random_sound_uget_string;
                                //std::cout << "poz 3\n\n";
                                
                            } 
                        }
                        sf::FloatRect aa = abc_a_button.getSpriteGlobalBounds();
                        if(aa.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            //choosed_sound = abc_a_button.button_string;
                            choosed_sound = a_text;
                            std::cout << choosed_sound+"\n";
                            if(sound_names[randomIndex_sound] == choosed_sound)
                            {
                                guessedsound_string = "RIGHT It's "+sound_names[randomIndex_sound];
                                guessedsound_text.setString(guessedsound_string);
                            }
                            if(sound_names[randomIndex_sound] != choosed_sound)
                            {
                                guessedsound_string = "WRONG It's "+sound_names[randomIndex_sound];
                                guessedsound_text.setString(guessedsound_string);
                            }
                        }
                        sf::FloatRect bb = abc_b_button.getSpriteGlobalBounds();
                        if(bb.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
            
                            choosed_sound = b_text;
                            if(sound_names[randomIndex_sound] == choosed_sound)
                            {
                                guessedsound_string = "RIGHT It's "+sound_names[randomIndex_sound];
                                guessedsound_text.setString(guessedsound_string);
                            }
                            if(sound_names[randomIndex_sound] != choosed_sound)
                            {
                                guessedsound_string = "WRONG It's "+sound_names[randomIndex_sound];
                                guessedsound_text.setString(guessedsound_string);
                            }
                        }
                        sf::FloatRect cc = abc_c_button.getSpriteGlobalBounds();
                        if(cc.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                           
                            choosed_sound = c_text;
                            if(sound_names[randomIndex_sound] == choosed_sound)
                            {
                                guessedsound_string = "RIGHT It's "+sound_names[randomIndex_sound];
                                guessedsound_text.setString(guessedsound_string);
                            }
                            if(sound_names[randomIndex_sound] != choosed_sound)
                            {
                                guessedsound_string = "WRONG It's "+sound_names[randomIndex_sound];
                                guessedsound_text.setString(guessedsound_string);
                            }
                        }


                    }

                }
            }
            if(screen_number == 52)
            {
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect gamesin = gamesingame_button.getSpriteGlobalBounds();
                        if(gamesin.contains(float(event.mouseButton.x),(event.mouseButton.y)))
                        {
                            screen_number = 5;
                            points_game2 = 0;
                            text_points.setString("Points: "+std::to_string(points_game2));
                            los_game2 = tablicaNazw[GiveRandomIndex(12)];
                            text_los_game2.setString("Find: "+los_game2);
                            text_wybrananazwa.setString("You chose: ");
                        }
                        
                    }

                }
                sf::FloatRect showw = fretboard_fill_button.getSpriteGlobalBounds();
                if (showw.contains(float(event.mouseButton.x), float(event.mouseButton.y)))
                {
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            show_fr = 1;
                        } // Zmieniam wartosc miennej gdy przycisk jest trzymany (podpowiedz dzwiekow)
                    }
                    if (event.type == sf::Event::MouseButtonReleased)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            show_fr = 0;
                        }
                    }
                }
               for (int i = 0; i < 96; i++)
                {
                    sf::FloatRect frett = tablicaObiektow[i].getSpriteGlobalBounds();
                    if (frett.contains(float(event.mouseButton.x), float(event.mouseButton.y)))
                    {
                        if (event.type == sf::Event::MouseButtonPressed)
                        {
                            if (event.mouseButton.button == sf::Mouse::Left)
                            {
                                //std::cout << "najechano na " << i << std::endl;
                                wybrananazwa = tablicaObiektow[i].name;
                            // std::cout << wybrananazwa << std::endl;
                                text_wybrananazwa.setString("You chose: " + wybrananazwa);

                                if (wybrananazwa != los_game2)
                                {
                                    points_game2 = 0;
                                    text_points.setString("Points: " + std::to_string(points_game2));
                                }
                                else if (wybrananazwa == los_game2)
                                {
                                    points_game2 = points_game2 + 1;
                                    text_points.setString("Points: " + std::to_string(points_game2));
                                    los_game2 = tablicaNazw[GiveRandomIndex(12)];
                                    text_los_game2.setString("Find: " + los_game2);
                                    text_wybrananazwa.setString("You chose: ");
                                }
                            }
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
            background_sprite.draw(window);

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
            //float freq = GetFrequencyFromMicrophone();
            float freq = GetFrequencyFromMicrophone();
            freqText.setString("Czestotliwosc: " + std::to_string(freq));
            //std::cout << "czestotliwosc: " << freq << "\n";
            //Pobieranie czestotliwosci z mikrofonu i dopasowanie jej do dzwieku, zmiana pozycji lini zaleznie od czestotliwosci
        
            // E = 41.20Hz, A = 55Hz, D = 73.42H, G = 98Hz
            // E - 36,41,46,   A - 50,55,60, D - 68,73,80 G - 93,98,103
            if (freq >= 35 && freq < 48)   //E
            {
                line_y = 405;
                float k = 41.2-freq;
                line_x = W/2 - k*10;
            }
            else if (freq >= 48 && freq < 62)  //A
            {
                line_y = 340;
                float k = 55-freq;
                line_x = W/2 - k*10;

            }
            else if (freq >= 62 && freq < 80)   //D
            {
                line_y = 278;
                float k = 73.42-freq;
                line_x = W/2 - k*10;

            }
            else if (freq >= 80 && freq < 105)  //G
            {
                line_y = 223;
                float k = 98-freq;
                line_x = W/2 - k*10;

            }
        
            line_sprite.setPosition(line_x, line_y);
            //drawing
            window.clear();
            background_sprite.draw(window);
            
            //window.draw(tuner_sprite);
            tuner_sprite.draw(window);
            back_button.draw(window);
            window.draw(play_E);
            window.draw(play_A);
            window.draw(play_D);
            window.draw(play_G);
            window.draw(line_sprite);
            window.draw(freqText);
            window.draw(freqEADGText);
            window.display();
        }
        //TAB CREATOR
        if (screen_number == 2)
        {

            static auto lastHitTime = std::chrono::steady_clock::now(); 
            static auto runningStartTime = std::chrono::steady_clock::now();  
            static int hitCounting = 0;    // odliczanie petli po uderzeniu zeby wziac czestotliwosc po chwili
            
            const int maxVolumePoints = 500;

            //ALgorytm porownujacy okna - spectral flux
            if (tabCreatorRunning){
                // Odczyt czestotliwosci i energii miedzy dwoma oknami (spectral flux)
                float freq = GetFrequencyFromMicrophone();
                float vol = GetSFFrameFromMicrophone(); //energia
                volumeHistory.push_back(vol);
                frequencyHistory.push_back(freq);

                // Obliczenie czasu odczytu od poczatku tabulatury (do analizy wykresow)
                auto now = std::chrono::steady_clock::now();
                float timeSec = std::chrono::duration<float>(now - runningStartTime).count();  //przeliczenie minionego czasu na sekundy
                dtHistory.push_back(timeSec);

                std::vector<Note> machingNotes; // tymczasowy wektor do szukania pasujacych do dzwiekow czestotliwosci w kilku petlach bo moze sie roznie ustabilizowac (po ok. 300ms czestotliwosc jest poprawna)
                std::vector<Note> currentNotes;     // ostateczne odczytane dzwieki po killu petlach po wykryciu uderzenia

                const int N = 20;

                //--------- PROG ADAPTACYJNY ------------------
                history.push_back(vol);
                if (history.size() > N) history.erase(history.begin()); //history.erase usuwa najstarszy element begin i przesuwa w lewo elementy

                // obliczenie sredniej
                float mean = 0;
                for (float v : history) mean += v;
                mean /= history.size();

                // obliczenie odchylenia
                float var = 0;      //wariancja
                for (float v : history) var += (v - mean) * (v - mean);
                var /= history.size();

                float stddev = sqrt(var);   //odchylenie standardowe

                // prog adaptacyjny jako srednia + 0.8odchylenia standardowego
                float threshold = mean + 0.8f * stddev;

                // cooldown
                static auto lastHit = std::chrono::steady_clock::now();
                now = std::chrono::steady_clock::now();
                float dt = std::chrono::duration<float>(now - lastHit).count();

                // detekcja uderzenia
                bool hit = false;
                if ((vol > threshold || vol > 0.5f)&& dt > 0.060f && vol > 0.05f) {   // 60 ms musi byc miedzy uderzeniami
                    hit = true;
                    if (vol > threshold ){std::cout << "threshold";}
                    if (vol > 0.5f ){std::cout << "0.5";}
                    lastHit = now;
                }

                //std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
                if (hit){
                    hitCounting = 4;        //szukam w kilku nastepnych petlach pasujacych dzwiekow
                    lastHitTime = now;
                    
                }
                if (hitCounting>0){
                    hitCounting--;
                    machingNotes = findMatchingNotes(freq);
                    if(hitCounting==0){
                        currentNotes = machingNotes;
                        std::cout << "\n wykryto dzwiek o freq: " << freq << "\n";
                        
                    }
                    
                }
                
                //Przypisanie podstawowych stringow jako kolejna czesc tabulatury
                std::string Estr = "---";
                std::string Astr = "---";
                std::string Dstr = "---";
                std::string Gstr = "---";

                //Jesli sa jakies znalezione dzwieki na strunach to zamien stringi na takie z tymi dzwiekami
                for (Note n: currentNotes){
                      std::string Str = "-" + std::to_string(n.fret);
                    if (n.fret < 10) {   // jesli liczba jednocyfrowa, dopisz "-"
                        Str += "-";
                    }
                    if (n.stringName == 'E'){ Estr = Str;}
                    if (n.stringName == 'A'){ Astr = Str;}
                    if (n.stringName == 'D'){ Dstr = Str;}
                    if (n.stringName == 'G'){ Gstr = Str;}

                }
                //Dopisanie kolejnej czesci tabluatury do stringow dla kazdej struny
                myTabs.E += Estr;
                myTabs.A += Astr;
                myTabs.D += Dstr;
                myTabs.G += Gstr;
                //std::cout <<myTabs.E.size();
                // Ronienie kolejnej lini zeby nie leciala tabulatura poza ekran tylko kolejne linie zaczynaly sie nizej
                if (myTabs.E.size() % 137 == 0){myTabs.E += "\n\n\n\n\n\n\n\n\nE|";}
                if (myTabs.A.size() % 137 == 0){myTabs.A += "\n\n\n\n\n\n\n\n\nA|";}
                if (myTabs.D.size() % 137 == 0){myTabs.D += "\n\n\n\n\n\n\n\n\nD|";}
                if (myTabs.G.size() % 137 == 0){myTabs.G += "\n\n\n\n\n\n\n\n\nG|";}
                if (myTabs.E.size() >=552 ){tabCreatorRunning = false;}
                tabCreatorEText.setString(myTabs.E);
                tabCreatorAText.setString(myTabs.A);
                tabCreatorDText.setString(myTabs.D);
                tabCreatorGText.setString(myTabs.G);
                std::cout << "vol: " << vol << std::endl;
                for (Note n: currentNotes){
                std::cout << n.stringName << n.fret <<std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); //usypiam watek na 100ms zeby petla nie leciala za szybko i sie nie wieszalo 
            }
            
            //drawing
            window.clear();
            background_sprite.draw(window);
            tabCreatorStartButton.draw(window);
            tabCreatorStopButton.draw(window);
            tabCreatorSaveButton.draw(window);
            tabCreatorResetButton.draw(window);

            //Rysowanie zielonego wykresu spectral flux na dole ekranu (duzy pik oznacza uderzenie)
            if (!volumeHistory.empty()) {
                sf::VertexArray lines(sf::LineStrip, volumeHistory.size());
                for (size_t i = 0; i < volumeHistory.size(); i++) {
                    float x = (float)i * (window.getSize().x / (float)maxVolumePoints);
                    float y = window.getSize().y - volumeHistory[i] * 50; // skalowanie do okna
                    if (y < 0) y = 0;
                    if (y > window.getSize().y) y = window.getSize().y;
                    lines[i] = sf::Vertex(sf::Vector2f(x, y), sf::Color::Green);
                }
                window.draw(lines);
            }
            window.draw(tabCreatorGText);
            window.draw(tabCreatorDText);
            window.draw(tabCreatorAText);
            window.draw(tabCreatorEText);
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
            //metronome_top_text.settext(beatsPerMeasureStr);
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
            background_sprite.draw(window);
            window.draw(metronome_top_text);
            //metronome_top_text.draw(window);
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
            tabs_button.draw(window);
            window.display();
        }
        //matronome tabs
        if (screen_number == 31)
        {
           // RYSOWANIE
            window.clear();
            metronome_tabs_sprite.draw(window);
            back_metronome_button.draw(window);
            window.display();
        }
        // BOOK
        if (screen_number == 4)
        {
            //
            //drawing
            window.clear();
            if (bookPage == 1){background_bookChords_sprite.draw(window);}
            if (bookPage == 2){background_bookScales_sprite.draw(window);}
            if (bookPage == 3){background_bookIntervals_sprite.draw(window);}
            if (bookPage == 4){background_bookShapes_sprite.draw(window);}
            if (bookPage == 5){background_bookCircle_sprite.draw(window);}
            if (bookPage == 6){background_bookRules_sprite.draw(window);}
            if (bookPage == 7){background_bookTabs_sprite.draw(window);}
            bookChordsButton.draw(window);
            bookScalesButton.draw(window);
            bookIntervalsButton.draw(window);
            bookShapesButton.draw(window);
            bookCircleButton.draw(window);
            bookRulesButton.draw(window);
            bookTabsButton.draw(window);
            back_button.draw(window);
            window.display();
        }
        // GAMES
        if (screen_number == 5)
        {
            //e1_sound.play_sound();

            //drawing
            window.clear();
            //window.draw(background_clear_sprite);
            background_clear_sprite.draw(window);
            window.draw(games_text);
            back_button.draw(window);
            game1_button.draw(window);
            game2_button.draw(window);
            window.display();

        }
        //Game 1
        if (screen_number == 51)
        {
            
                //drawing
            window.clear();
            //window.draw(background_clear_sprite);
            background_clear_sprite.draw(window);
            window.draw(games_text);
            gamesingame_button.draw(window);
            random_sound_button.draw(window);
            abc_a_button.draw(window);
            abc_b_button.draw(window);
            abc_c_button.draw(window);
            window.draw(guessedsound_text);
            window.display();
        }
        //Game 2
        if (screen_number == 52)
        {
            int d = GiveRandomIndex(11);
            std::string wylosowany_dzwiek = tablicaNazw[d];


            window.clear();
            //window.draw(background_clear_sprite);
            background_clear_sprite.draw(window);
            fretboard_clear_sprite.draw(window);
            //window.draw(games_text);
            gamesingame_button.draw(window);
            fretboard_fill_button.draw(window);
            window.draw(text_los_game2);
            window.draw(text_wybrananazwa);
            window.draw(text_points);
            for(int i = 0; i<96; i++)
            {
                 tablicaObiektow[i].draw(window);
            }
            if(show_fr)
            {
                freatboard_filled_sprite.draw(window);
            }
            window.display();
        }

    }

    return 0;
}
