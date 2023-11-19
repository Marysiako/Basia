#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>

double CalculateFrequency(const std::vector<double>& samples);
double GetFrequencyFromMicrophone();
int GiveRandomIndex(int i);

#endif
