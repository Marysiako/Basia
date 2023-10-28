#include <iostream>
#include <vector>
#include <cmath>
#include <portaudio.h>

const int SAMPLE_RATE = 44100;
const int FRAME_SIZE = 1024;

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

int main() {
    PaError err;

    // Inicjalizacja PortAudio.
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Błąd inicjalizacji PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Otwarcie strumienia wejściowego.
    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE, FRAME_SIZE, NULL, NULL);
    if (err != paNoError) {
        std::cerr << "Błąd otwierania strumienia: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Błąd uruchamiania strumienia: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    std::vector<double> samples(FRAME_SIZE);

    while (true) {
        err = Pa_ReadStream(stream, samples.data(), FRAME_SIZE);
        if (err != paNoError) {
            std::cerr << "Błąd odczytu danych z mikrofonu: " << Pa_GetErrorText(err) << std::endl;
            break;
        }

        double frequency = CalculateFrequency(samples);

        std::cout << "Dominująca częstotliwość: " << frequency << " Hz" << std::endl;
    }

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

    return 0;
}
