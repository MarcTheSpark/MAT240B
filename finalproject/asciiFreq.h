#ifndef _ASCII_FREQ
#define _ASCII_FREQ

#include <string>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <regex>
#include <cmath>

struct ASCIIFreqMap {

  unordered_map<char, float> letterFrequencyDictionary;

  ASCIIFreqMap() {
    string line;
    ifstream asciiFreqFile ("asciiFreq.txt");

    float maxFrequency = 0;
    if (asciiFreqFile.is_open())
    {
      while ( getline (asciiFreqFile, line) )
      {
        regex configPattern(R"((.)\s*(.*))");
        auto line_begin = std::sregex_iterator(line.begin(), line.end(), configPattern);
        auto line_end = std::sregex_iterator();

        // Question: what the hell does ++i mean on such a weird object
        for(sregex_iterator i = line_begin; i != line_end; ++i) {
          smatch match = *i;
          char character = match.str(1)[0];
          float frequency = atof(string(match.str(2)).c_str());
          letterFrequencyDictionary[character] = frequency;
          if (frequency > maxFrequency) maxFrequency = frequency;
        }
      }
      asciiFreqFile.close();
    }
    for (auto it : letterFrequencyDictionary) {
      letterFrequencyDictionary[it.first] = sqrt(it.second / maxFrequency);
    }
  }
};

#endif
