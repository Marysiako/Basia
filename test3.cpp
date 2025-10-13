/*
TEN TEST POPRAWNIE ODCZYTUJE CZESTOTLIWOSCI DZWIEKOW GRANYCH NA GITARZE BASOWEJ
ORAZ POKAZUJE NA BIERZACO GRANA FALE DZWIEKOWA I PIK PO FFT

KOMPILOWANIE
    g++ test3.cpp -lsfml-graphics -lsfml-window -lsfml-system -lportaudio -lfftw3f -pthread
*/ 

#include <SFML/Graphics.hpp>
#include <portaudio.h>
#include <vector>
#include <iostream>
#include <mutex>
#include <fftw3.h>
#include <cmath>
#include <iomanip>
#include <sstream>

#define SAMPLE_RATE 44100   // czestotliwosc probkowania
#define FRAMES_PER_BUFFER 256 // liczba probek na jeden callback
#define BUFFER_SIZE 32768   // rozmiar bufora - nim wiekszy tym dokladniej

// bufor na probki audio
std::vector<float> g_samples(BUFFER_SIZE, 0.0f);
std::mutex g_mutex;         // zabezpiecza przed jednoczesnym dostepem z dwoch watkow (zapiywaniem i czytaniem danych do wizualizacji wykresu)



// Callback PortAudio - obsluga
static int audioCallback(const void *inputBuffer, void *, unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *) {
    const float *in = static_cast<const float*>(inputBuffer);
    if (!in) return paContinue; // jesli jest cisza kontynuuj

    std::lock_guard<std::mutex> lock(g_mutex);  //lock_guard blokuje tylko na czas bycia w {} i sam odblokowuje
    std::move(g_samples.begin() + framesPerBuffer, g_samples.end(), g_samples.begin()); //przesuwam w lewo o liczbe probek 
    std::copy(in, in + framesPerBuffer, g_samples.end() - framesPerBuffer); //dodanie nowych danych na koniec bufora

    return paContinue;
}

int main() {
    // ------ PortAudio ------
    Pa_Initialize();
    PaStream *stream;
    PaError err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE,
                                       FRAMES_PER_BUFFER, audioCallback, nullptr);      //otwarcie strunienia ktory wywoluje ciagle callback
    if (err != paNoError) {
        std::cerr << "Blad PortAudio: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }
    Pa_StartStream(stream);

    // ------ SFML ------
    sf::RenderWindow window(sf::VideoMode(1000, 400), "Dominant Frequency");
    sf::VertexArray waveform(sf::LineStrip, BUFFER_SIZE);   //fala dzwieku
    sf::VertexArray spectrum(sf::LineStrip, BUFFER_SIZE / 2);   //po fft (fast fourier transform)
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Nie udało się wczytać fontu!\n";
    }
    sf::Text freqText("", font, 20);
    freqText.setFillColor(sf::Color::Red);
    freqText.setPosition(10, 10);

    // ------ FFT setup ------
    std::vector<float> fft_in(BUFFER_SIZE);
    std::vector<float> fft_mag(BUFFER_SIZE / 2); //wektor na magnitudy po transformacji /2 bo polowa taka sama
    fftwf_complex* fft_out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (BUFFER_SIZE/2 + 1)); 
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(BUFFER_SIZE, fft_in.data(), fft_out, FFTW_ESTIMATE);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed) window.close();

        {
            std::lock_guard<std::mutex> lock(g_mutex);
            fft_in = g_samples;
        }

        // okna Hanninga - wygladzenie poczatku i konca probki zeby mialy wartosc 0 zeby wyeliminowac falszywe czestotliwosci
        for (int i = 0; i < BUFFER_SIZE; i++)
            fft_in[i] *= 0.5f * (1 - cos(2 * M_PI * i / (BUFFER_SIZE - 1)));

        // ------ FFT ------
        fftwf_execute(plan);

        // Obliczenie modulu widma
        for (int i = 0; i < BUFFER_SIZE / 2; i++) {
            fft_mag[i] = std::sqrt(                 //pierwiastek z(Re^2 + Im^2) czyli faktyczna amplituda/magnituda
                fft_out[i][0] * fft_out[i][0] +
                fft_out[i][1] * fft_out[i][1]
            );
        }

        // Maksymalna amplituda do ustalenia progu
        float maxMag = *std::max_element(fft_mag.begin(), fft_mag.end());
        float threshold = maxMag * 0.15f; //prog 15% od maksymalnej amplitudy - ignoruje szumy

        int firstPeakIndex = 0;
        float prevVal = 0.0f;
        bool found = false;

        // Szukam pierwszego wyraznego piku ponizej 200 Hz (ograniczam zakres dla basu)
        for (int i = 1; i < BUFFER_SIZE / 2; i++) {
            float freq = i * (float)SAMPLE_RATE / BUFFER_SIZE; //obliczenie odpowiadajacej kazdemu i czestotliwosci
            if (freq > 200.0f) break; // nie interesuja mnie wyzsze harmoniczne

            // Detektor lokalnego maksimum powyzej progu czyli pierwszego silnego piku (wiekszy niz prog, rosnie i wiekszy niz nastepny)
            if (fft_mag[i] > threshold && fft_mag[i] > prevVal &&
                fft_mag[i] > fft_mag[i + 1]) {
                firstPeakIndex = i;
                found = true;
                break;
            }
            prevVal = fft_mag[i];
        }

        // Jesli nie znaleziono pierwszego piku, wez najwyzszy
        if (!found) {
            firstPeakIndex = std::distance(     //distance zwraca index maximum
                fft_mag.begin(),
                std::max_element(fft_mag.begin(), fft_mag.begin() + BUFFER_SIZE / 2)
            );
        }

        float dominantFreq = firstPeakIndex * (float)SAMPLE_RATE / BUFFER_SIZE;
        
        // ------ Fala dzwiekowa ------
        for (size_t i = 0; i < g_samples.size(); i++) {
            float x = static_cast<float>(i) * (500.0f / BUFFER_SIZE);
            float y = 200 - g_samples[i] * 150;
            waveform[i].position = sf::Vector2f(x, y);
            waveform[i].color = sf::Color::Cyan;
        }

        // ------ FFT ------
        for (size_t i = 0; i < fft_mag.size(); i++) {
            float x = 500 + static_cast<float>(i) * (500.0f / fft_mag.size());
            float y = 200 - std::log1p(fft_mag[i])*20;
            spectrum[i].position = sf::Vector2f(x, y);
            spectrum[i].color = sf::Color::Yellow;
        }

        // ------ Wyswietlenie czestotliwosci ------
        std::ostringstream ss
        ss << std::fixed << std::setprecision(1) << dominantFreq << " Hz";  //format zmienno przecinkowy z 1 liczba po przecinku
        freqText.setString(ss.str());

        window.clear(sf::Color::Black);
        window.draw(waveform);
        window.draw(spectrum);
        window.draw(freqText);
        window.display();
    }

    fftwf_destroy_plan(plan);
    fftwf_free(fft_out);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
}