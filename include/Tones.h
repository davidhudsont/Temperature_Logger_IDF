

#include "Notes.h"

#include <array>
#include <vector>
#include <stdint.h>

struct Tone
{
    int note;
    int duration;
};

std::vector<Tone> simpleTone = {
    {NOTE_C4, 500},
    {NOTE_D4, 500},
    {NOTE_E4, 500},
    {NOTE_F4, 500},
    {NOTE_G4, 500},
    {NOTE_A4, 500},
    {NOTE_B4, 500},
};