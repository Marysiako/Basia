#include "functions.h"
#include <cmath>
#include <portaudio.h>
#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <cstdlib>

#include <mutex>
#include <fftw3.h>
#include <iomanip>
#include <sstream>


//DO ODCZYTU CZESTOTLIWOSCI
#define SAMPLE_RATE 44100   // czestotliwosc probkowania
#define FRAMES_PER_BUFFER 256 // liczba probek na jeden callback
#define BUFFER_SIZE 32768   // rozmiar bufora - nim wiekszy tym dokladniej

// bufor na probki audio
std::vector<float> g_samples(BUFFER_SIZE, 0.0f);
std::mutex g_mutex;         // zabezpiecza przed jednoczesnym dostepem z dwoch watkow (zapiywaniem i czytaniem danych do wizualizacji wykresu)

// Callback PortAudio - obsluga (Ppobiera probke audio i zaspisuje do g_samples)
static int audioCallback(const void *inputBuffer, void *, unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *) {
    const float *in = static_cast<const float*>(inputBuffer);
    if (!in) return paContinue; // jesli jest cisza kontynuuj

    std::lock_guard<std::mutex> lock(g_mutex);  //lock_guard blokuje tylko na czas bycia w {} i sam odblokowuje
    std::move(g_samples.begin() + framesPerBuffer, g_samples.end(), g_samples.begin()); //przesuwam w lewo o liczbe probek 
    std::copy(in, in + framesPerBuffer, g_samples.end() - framesPerBuffer); //dodanie nowych danych na koniec bufora

    return paContinue;
}
PaStream* stream = nullptr;

bool InitAudio() {
    Pa_Initialize();
    PaError err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE,
                                       FRAMES_PER_BUFFER, audioCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << "\n";
        return false;
    }
    Pa_StartStream(stream);
    return true;
}

void CloseAudio() {
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
}
float GetFrequencyFromMicrophone() {

        // ------ FFT setup ------
    std::vector<float> fft_in(BUFFER_SIZE);
    std::vector<float> fft_mag(BUFFER_SIZE / 2); //wektor na magnitudy po transformacji /2 bo polowa taka sama
    fftwf_complex* fft_out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (BUFFER_SIZE/2 + 1)); 
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(BUFFER_SIZE, fft_in.data(), fft_out, FFTW_ESTIMATE);

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
        
        //INTERPOLACJA dla dokladniejszej czestotliwosci
        int i = firstPeakIndex;
        // SPrawdzenie czy nie jest na brzegu tablicy
        if (i > 0 && i < (BUFFER_SIZE / 2 - 1)) {
            float left = fft_mag[i - 1];
            float center = fft_mag[i];
            float right = fft_mag[i + 1];

            // Wzor na przesuniecie (moze byc ujemne)
            float delta = 0.5f * (left - right) / (left - 2.0f * center + right);

            //Dokladniejsza czestotliwosc
            float interpolatedIndex = i + delta;
            dominantFreq = interpolatedIndex * (float)SAMPLE_RATE / BUFFER_SIZE;

        }

        
        fftwf_destroy_plan(plan);
        fftwf_free(fft_out);
        return dominantFreq;
}
float GetVolumeFromMicrophone(){
    int volumeBufferSize = 2*256; //
    std::vector<float> fft_in(volumeBufferSize);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        //fft_in = g_samples;
        fft_in =  std::vector<float>(g_samples.end() - volumeBufferSize, g_samples.end());
    }
    // Odczyt glosnosci -------------------------------
        float volume = 0.0f;
        {
            //std::lock_guard<std::mutex> lock(g_mutex);

            // RMS (Root Mean Square) - pierwiastek z wartosci sredniej kwadratu, czyli srednia amplituda sygnalu
            float sumSquares = 0.0f;
            for (float s : fft_in)
                sumSquares += s * s;

            volume = std::sqrt(sumSquares / fft_in.size());
        
        }
       return volume; 
}
    
int GiveRandomIndex(int i)
{
    //Funkcja losuje numer z zakresu 0-i
    size_t randomIndex = std::rand() % i;
    return randomIndex;
}

