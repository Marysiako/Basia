#include "functions.h"
#include <cmath>
#include <portaudio.h>
#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <fftw3.h>
// 40 -312 hz
const int SAMPLE_RATE = 44100;
const int FRAME_SIZE = 1024;
const double VOLUME_THRESHOLD = 0.001;  // Próg głośności
//const int MIN_FREQUENCY = 10;
//const int MAX_FREQUENCY = 400;
/*
double GetFrequencyFromMicrophone() {
    PaError err;

    // Inicjalizacja PortAudio
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Błąd inicjalizacji PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return -1.0;
    }

    // Otwarcie strumienia wejściowego
    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE, FRAME_SIZE, NULL, NULL);
    if (err != paNoError) {
        std::cerr << "Błąd otwierania strumienia: " << Pa_GetErrorText(err) << std::endl;
        return -1.0;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd uruchamiania strumienia: " << Pa_GetErrorText(err) << std::endl;
        return -1.0;
    }

    std::vector<double> samples(FRAME_SIZE);

    err = Pa_ReadStream(stream, samples.data(), FRAME_SIZE);
    if (err != paNoError) {
        std::cerr << "Błąd odczytu danych z mikrofonu: " << Pa_GetErrorText(err) << std::endl;
        return -1.0;
    }

    fftw_complex* fftInput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FRAME_SIZE);
    fftw_complex* fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FRAME_SIZE);

    for (int i = 0; i < FRAME_SIZE; ++i) {
        fftInput[i][0] = samples[i]; // Real part
        fftInput[i][1] = 0.0;         // Imaginary part (assuming real input)
    }

    fftw_plan plan = fftw_plan_dft_1d(FRAME_SIZE, fftInput, fftOutput, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);


    // Szukanie dominujacej czestotliwosci
    double maxMagnitude = 0.0;
    int maxIndex = 0;

    for (int i = 0; i < FRAME_SIZE; ++i) {
        double magnitude = sqrt(fftOutput[i][0] * fftOutput[i][0] + fftOutput[i][1] * fftOutput[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxIndex = i;
        }
    }

    double sampleRate = SAMPLE_RATE;
    double frequency = (maxIndex * sampleRate) / FRAME_SIZE;

    // Zamykanie strumienia
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd zatrzymywania strumienia: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd zamykania strumienia: " << Pa_GetErrorText(err) << std::endl;
    }

    // Terminate PortAudio.
    Pa_Terminate();

    return frequency;
}
*/

double GetFrequencyFromMicrophone() {
    PaError err;

    // Inicjalizacja PortAudio
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Błąd inicjalizacji PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return -1.0;
    }

    // Otwarcie strumienia wejściowego
    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE, FRAME_SIZE, NULL, NULL);
    if (err != paNoError) {
        std::cerr << "Błąd otwierania strumienia: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return -1.0;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd uruchamiania strumienia: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return -1.0;
    }

    // Odczyt danych z mikrofonu
    std::vector<float> samples(FRAME_SIZE);
    err = Pa_ReadStream(stream, samples.data(), FRAME_SIZE);
    if (err != paNoError) {
        std::cerr << "Błąd odczytu danych z mikrofonu: " << Pa_GetErrorText(err) << std::endl;
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        return -1.0;
    }

    // Sprawdzanie głośności
    double volume = 0.0;
    for (int i = 0; i < FRAME_SIZE; ++i) {
        volume += std::abs(samples[i]);
    }
    volume /= FRAME_SIZE;

    if (volume < VOLUME_THRESHOLD) {
        std::cout << "Brak wystarczającej głośności." << std::endl;
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        return -1.0;
    }

    // Przygotowanie danych do FFT
    fftw_complex* fftInput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FRAME_SIZE);
    fftw_complex* fftOutput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FRAME_SIZE);

    for (int i = 0; i < FRAME_SIZE; ++i) {
        fftInput[i][0] = samples[i]; // Część rzeczywista
        fftInput[i][1] = 0.0;         // Część urojona (dla danych rzeczywistych)
    }

    // Wykonanie FFT
    fftw_plan plan = fftw_plan_dft_1d(FRAME_SIZE, fftInput, fftOutput, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    // Znalezienie dominującej częstotliwości
    double maxMagnitude = 0.0;
    int maxIndex = 0;

    for (int i = 0; i < FRAME_SIZE; ++i) {
        double magnitude = sqrt(fftOutput[i][0] * fftOutput[i][0] + fftOutput[i][1] * fftOutput[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxIndex = i;
        }
    }

    double sampleRate = SAMPLE_RATE;
    double frequency = (maxIndex * sampleRate) / FRAME_SIZE;

    // Zamknięcie strumienia
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd zatrzymywania strumienia: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd zamykania strumienia: " << Pa_GetErrorText(err) << std::endl;
    }

    // Zwolnienie pamięci
    fftw_free(fftInput);
    fftw_free(fftOutput);

    // Zakończenie PortAudio.
    Pa_Terminate();

    return frequency;
}

int GiveRandomIndex(int i)
{
    //Funkcja losuje numer z zakresu 0-i
    size_t randomIndex = std::rand() % i;
    return randomIndex;
}
