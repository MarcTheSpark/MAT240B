#include "everything.h"
#include <vector>
using namespace diy;
using namespace std;


struct DelayLine : vector<float> {
  unsigned index{0};

  DelayLine(unsigned size) {
    resize(size);
  }

  void tap_in(float value) {
    at(index) = value;
    index++;
    if (index >= size()) index -= size();
  }

  float tap_out(unsigned delay) {
    if (delay > size()) {
      std::cerr << "Delay of " << delay
                << " too large for DelayLine of length " << size() << '\n';
      exit(-1);
    }
    int read_location = index - delay;
    if (read_location < 0) read_location += size();
    return at(read_location);
  }

  float tap_out_f(float delay) {
    if (delay == int(delay)) {
      return tap_out(int(delay));
    } else {
      float fractpart = delay - int(delay);
      return fractpart * tap_out(int(delay) + 1) +
             (1 - fractpart) * (tap_out(int(delay)));
    }
  }

  void print_contents() {
    for (const auto& i: *this) cout << i << ' ';
    cout << endl;
  }

};

struct StringSimulator : DelayLine {
  float freq, lastSamp{0}, decayFactor=0.95;

  StringSimulator(float frequency=440, float minFrequency=20) :
  DelayLine(int(SAMPLE_RATE / minFrequency)), freq(frequency)
  {}

  float operator ()() {
    float delayedSamp = tap_out_f(SAMPLE_RATE / freq);
    float output = (delayedSamp + lastSamp) / 2 * decayFactor;
    lastSamp = delayedSamp;
    tap_in(output);
    return output;
  }

  void pluck(float stringLoc) {
    // fill 5% of the delay line with noise starting at stringLoc where
    // a stringLoc of 0 puts in the middle and 1 puts it on the edge
    int startOfNoise = index - int(SAMPLE_RATE / freq * (0.5 + stringLoc / 2));
    int endOfNoise = startOfNoise + int(SAMPLE_RATE / freq * 0.1);
    for(int k = startOfNoise; k < endOfNoise; k++) {
      int writeLocation = (k < 0) ? k + size() : k;
      float envelopePosition = float(k - startOfNoise) / (endOfNoise - startOfNoise);
      at(writeLocation) += uniform(-1, 1) *
        ((envelopePosition > 0.5) ? 2 * (1 - envelopePosition): 2* envelopePosition);
    }
  }
};

int main(int argc, char const *argv[]) {
  /* code */
  StringSimulator string;
  string.freq = 220;

  for(float pluckLoc = 0; pluckLoc <= 1.0; pluckLoc += 0.05) {
    // pluckLoc doesn't seem to work :-(
    string.pluck(pluckLoc);
    for(int i=0; i < 10000; i++) {
      say(string());
    }
  }
  return 0;
}
