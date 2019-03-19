#ifndef _LETTER_FREQUENCY_MAP
#define _LETTER_FREQUENCY_MAP

#include <unordered_map>  // unordered_map
#include <algorithm>      // sort
#include <utility>

struct LetterFrequencyMap {
  unordered_map<char, float> letterFrequencyDictionary;

  LetterFrequencyMap(string inputString) {

    // first count up how many of each letter and put in our unordered map temporarily
    for(char& c: inputString) {
      if (letterFrequencyDictionary.count(c) > 0) {
        letterFrequencyDictionary[c]++;
      } else {
        letterFrequencyDictionary[c] = 1;
      }
    }

    // then transfer to a vector of pairs and sort according to frequency
    vector<pair<char, float>> letterCount;
    for(auto& it: letterFrequencyDictionary) letterCount.push_back({it.first, it.second});
    sort(letterCount.begin(), letterCount.end(),
      [](pair<char, float> x, pair<char, float> y){ return x.second < y.second; }
    );

    /* we want each letter to get a unique frequency value (so that they all
     map to different grains), so let's alter the count so that when a letter
     has the same count as the one before it everything there and after gets
     shifted up by one */
    int shift = 0;
    int lastCount = -1;
    for(auto& x: letterCount) {
      x.second += shift;
      if (x.second == lastCount) {
        x.second++;
        shift++;
      }
      lastCount = x.second;
    }

    // finally, go back to the dictionary for easy lookup
    letterFrequencyDictionary.clear();
    for(auto& x: letterCount) {
      letterFrequencyDictionary[x.first] = float(x.second) / lastCount;
    }

  }
};

#endif
