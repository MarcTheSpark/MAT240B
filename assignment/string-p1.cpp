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
  float freq, lastSamp{0};

  StringSimulator(float frequency=440, float minFrequency=20) :
  DelayLine(int(SAMPLE_RATE / minFrequency)), freq(frequency)
  {}

  float operator ()() {
    float delayedSamp = tap_out_f(SAMPLE_RATE / freq);
    float output = (delayedSamp + lastSamp) / 2;
    lastSamp = delayedSamp;
    tap_in(output);
    return output;
  }

  void pluck(float stringLoc) {
    uniform(-1, 1);
  }

};

int main(int argc, char const *argv[]) {
  /* code */
  StringSimulator string;
  string.at(string.size()-1) = 1.0;
  for(int i=0; i < 204; i++) {
    say(string());
  }
  return 0;
}
