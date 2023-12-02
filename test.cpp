#include <iostream>
#include <cmath>
#include <vector>
#include "portaudio.h"

const int SAMPLE_RATE = 44100;
const int FRAME_SIZE = 1024;
const double PI = 3.14159265358979323846;

// Funkcja do obliczania częstotliwości na podstawie próbek dźwięku.
double CalculateFrequency(const std::vector<double>& samples) {
    double maxAmplitude = 0.0;
    int maxIndex = 0;

    for (int i = 0; i < FRAME_SIZE; ++i) {
        if (samples[i] > maxAmplitude) {
            maxAmplitude = samples[i];
            maxIndex = i;
        }
    }

    // Oblicz częstotliwość na podstawie indeksu próbki i częstotliwości próbkowania.
    double frequency = static_cast<double>(maxIndex) * SAMPLE_RATE / FRAME_SIZE;

    return frequency;
}

// Callback dla PortAudio do przetwarzania próbek dźwięku.
int RecordCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
                   const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    const float* in = static_cast<const float*>(inputBuffer);
    std::vector<double> samples(FRAME_SIZE);

    for (int i = 0; i < FRAME_SIZE; ++i) {
        samples[i] = static_cast<double>(in[i]);
    }

    double frequency = CalculateFrequency(samples);

    // Prosta detekcja tonu stroikiem gitary.
    if (frequency > 80.0 && frequency < 1500.0) {
        std::cout << "Aktualna częstotliwość: " << frequency << " Hz" << std::endl;

        // Możesz dodać kod do porównywania częstotliwości z oczekiwanymi wartościami tonów gitary
        // i informować użytkownika, czy gitara jest dobrze nastrojona.
    }

    return paContinue;
}

int main() {
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Błąd inicjalizacji PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return -1;
    }

    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE, FRAME_SIZE, RecordCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "Błąd otwierania strumienia: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return -1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd uruchamiania strumienia: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return -1;
    }

    std::cout << "Naciśnij Enter, aby zakończyć strojenie gitary..." << std::endl;
    std::cin.get();

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd zatrzymywania strumienia: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd zamykania strumienia: " << Pa_GetErrorText(err) << std::endl;
    }

    Pa_Terminate();

    return 0;
}