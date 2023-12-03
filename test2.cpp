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
const int MIN_FREQUENCY = 10;
const int MAX_FREQUENCY = 400;

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
    
    //if (frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY) {
    //    return -1.0;  // Return an invalid frequency outside the range.
    //}

    return frequency;
}

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
int main() {

    for (int i=0; i<10; i++)
    {
        double frequency = GetFrequencyFromMicrophone();
    std::cout << frequency << std::endl;
    }
    

}
