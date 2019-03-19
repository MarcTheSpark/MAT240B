#include "utils/utils.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include "configparser.h"
#include "grainRepository.h"
#include "stringRecombinator.h"
#include "letterFrequencyMap.h"
#include "utils/writeWaveform.h"
#include <time.h>
#include <regex>
using namespace diy;
using namespace std;

struct GrainPlayer {
  vector<float> samps;
  int index = 0;
  bool done = false;
  GrainPlayer(vector<float> samples) : samps(samples) {}

  float next() {
    if (index >= samps.size()) {
      done = true;
      return 0;
    } else {
      return samps.at(index++);
    }
  }
};

int main (int argc, char **argv)
{
  srand(time(NULL)); // seed the random generator

  StringGranulatorConfig config = parseConfigurations(argc, argv);
  GrainRepository grains(config.soundFilePath, config.grainLength);

  string delimitersRemoved = regex_replace(config.textToGranulate, regex(config.paragraphDelimiter), "");
  delimitersRemoved = regex_replace(delimitersRemoved, regex(config.sentenceDelimiter), "");
  delimitersRemoved = regex_replace(delimitersRemoved, regex(config.wordDelimiter), "");
  LetterFrequencyMap letterFrequencyMap(delimitersRemoved);

  StringRecombinator inputRecombinator(
    config.textToGranulate, config.wordDelimiter,
    config.sentenceDelimiter, config.paragraphDelimiter
  );

  string recombinedString = inputRecombinator.regenerate(
    config.wordStretch, config.sentenceStretch,
    config.paragraphStretch, config.documentStretch,
    config.wordSpacing, config.sentenceSpacing, config.paragraphSpacing
  );

  vector<pair<unsigned, GrainPlayer>> grainSchedule;

  unsigned grainStart = 0;

  cout << "Scheduling grains..." << endl;
  for (char c : recombinedString) {
    float grainLoudness;
    if(c == - 1) {
      grainLoudness = config.wordDelimiterGrainLoudness;
    } else if (c == -2) {
      grainLoudness = config.sentenceDelimiterGrainLoudness;
    } else if (c == -3) {
      grainLoudness = config.paragraphDelimiterGrainLoudness;
    } else {
      // note that the _least_ frequent grains are the _most_ loud and vice-versa
      grainLoudness = scale(letterFrequencyMap.letterFrequencyDictionary[c], 0, 1,
        config.contentGrainMaxLoudness, config.contentGrainMinLoudness);
      // grainLoudness = 1 - 0.4 * letterFrequencyMap.letterFrequencyDictionary[c];
    }
    // vector<float> windowedGrain = grains.getWindowedGrain(grainLoudness);
    GrainPlayer thisGrainPlayer(grains.getWindowedGrain(grainLoudness));


    grainSchedule.push_back(make_pair(grainStart, thisGrainPlayer));
    grainStart += grains.grainSampleLength / 2;
  }
  cout << "done." << endl;

  unsigned endSample = grainStart + grains.grainSampleLength;

  vector<float> outputSamples;

  cout << "Constructing output..." << endl;
  while(outputSamples.size() < endSample) {
    float outputSample = 0;
    for (int i=0; i < grainSchedule.size(); i++) {
      unsigned& grainStartSample = grainSchedule.at(i).first;
      GrainPlayer& grainPlayer = grainSchedule.at(i).second;

      if (outputSamples.size() >= grainStartSample && !grainPlayer.done) {
        outputSample += grainPlayer.next();
      }
    }
    if (outputSamples.size() % 10000 == 0) { cout << outputSamples.size()
      << "/" << endSample << " samples written" << endl;}
    outputSamples.push_back(outputSample);
  }
  cout << "done." << endl;


  writeWaveform(outputSamples, config.outputFileName);

  // for(float f = 1.0; f >= 0; f -= 0.05) {
  //   for (float samp: grains.getWindowedGrain(f)) {
  //     say(samp);
  //   }
  // }
  exit (0);
}
