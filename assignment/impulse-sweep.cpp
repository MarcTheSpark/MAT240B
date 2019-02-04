#include "everything.h"
#define NYQUIST (SAMPLE_RATE / 2)
using namespace diy;

void setIncrementAndHarmonicsFromFreq(float freq, float &inc, float &hCount) {
  inc = freq / SAMPLE_RATE;
  // I'm letting harmonic count be a floating point number, with the fractional
  // part representing the fading in of the final harmonic. This way harmonics
  // don't suddenly appear
  hCount = SAMPLE_RATE / 2 / freq;
}

int main(int argc, char* argv[]) {
  float frequency = NYQUIST * 0.9;
  float phase = 0;

  float increment, harmonicCount;
  setIncrementAndHarmonicsFromFreq(frequency, increment, harmonicCount);

  float v;

  while (frequency > mtof(0)) {
    v = 0;
    for (int h = 1; h < harmonicCount - 1; ++h)
      // all but the last harmonic are full value
      v += cos(2 * pi * h * phase);

    // last harmonic is faded in accoring to the fractional part of harmonicCount
    v += cos(2 * pi * int(harmonicCount) * phase) * (harmonicCount - int(harmonicCount));

    v /= harmonicCount;
    say(v);

    // increment phase
    phase += increment;
    // wrapping was necessary: without it, I got floating point precision errors
    if (phase > 1) phase--;

    frequency *= 0.9999;
    setIncrementAndHarmonicsFromFreq(frequency, increment, harmonicCount);
  }
}
