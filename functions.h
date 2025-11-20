#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>

bool InitAudio();
void CloseAudio();
float GetFrequencyFromMicrophone();
float GetVolumeFromMicrophone();
float GetSFFrameFromMicrophone();
int GiveRandomIndex(int i);
#endif
