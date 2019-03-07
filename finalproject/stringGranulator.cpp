#include "../../unix/everything.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <utility>
using namespace diy;
using namespace std;

// from https://stackoverflow.com/questions/16749069/c-split-string-by-regex
std::vector<std::string>
resplit(const std::string & s, std::string rgx_str = "\\s+") {
   std::vector<std::string> elems;
   std::regex rgx (rgx_str);

   std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
   std::sregex_token_iterator end;

   while (iter != end)  {
       //std::cout << "S43:" << *iter << std::endl;
       if (iter->length() > 0) elems.push_back(*iter);
       ++iter;
   }

   return elems;
}

// From synths.h
struct Array : std::vector<float> {
  void operator()(float f) {
    push_back(f);
    //
  }
  void save(const char* fileName) const {
    drwav_data_format format;
    format.channels = 1;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.sampleRate = 44100;
    format.bitsPerSample = 32;
    drwav* pWav = drwav_open_file_write(fileName, &format);
    if (pWav == nullptr) {
      std::cerr << "failed to write " << fileName << std::endl;
      drwav_close(pWav);
      return;
    }
    drwav_uint64 samplesWritten = drwav_write(pWav, size(), data());
    if (samplesWritten != size()) {
      std::cerr << "failed to write all samples to " << fileName << std::endl;
      drwav_close(pWav);
      return;
    }
    drwav_close(pWav);
  }

  bool load(const char* fileName) {
    drwav* pWav = drwav_open_file(fileName);
    if (pWav == nullptr) return false;

    if (pWav->channels == 1) {
      resize(pWav->totalPCMFrameCount);
      drwav_read_f32(pWav, size(), data());  // XXX does fail?
      drwav_close(pWav);
      return true;
    }

    if (pWav->channels > 1) {
      float* pSampleData = (float*)malloc((size_t)pWav->totalPCMFrameCount *
                                          pWav->channels * sizeof(float));
      drwav_read_f32(pWav, pWav->totalPCMFrameCount, pSampleData);
      drwav_close(pWav);

      resize(pWav->totalPCMFrameCount);
      for (unsigned i = 0; i < size(); ++i)
        at(i) = pSampleData[pWav->channels * i];  // only read the first channel
      return true;
    }

    return false;
  }

  // raw lookup
  // except that i think "at" does bounds checking
  float raw(const float index) const {
    const unsigned i = floor(index);
    const float x0 = at(i);
    const float x1 = at((i == (size() - 1)) ? 0 : i + 1);  // looping semantics
    const float t = index - i;
    return x1 * t + x0 * (1 - t);
  }

  // void resize(unsigned n) { data.resize(n, 0); }
  // float& operator[](unsigned index) { return data[index]; }

  // allow for sloppy indexing (e.g., negative, huge) by fixing the index to
  // within the bounds of the array
  float get(float index) const {
    if (index < 0) index += size();
    if (index > size()) index -= size();
    return raw(index);  // defer to our method without bounds checking
  }
  float operator[](const float index) const { return get(index); }
  float phasor(float index) const { return get(size() * index); }

  void add(const float index, const float value) {
    const unsigned i = floor(index);
    const unsigned j = (i == (size() - 1)) ? 0 : i + 1;  // looping semantics
    const float t = index - i;
    at(i) += value * (1 - t);
    at(j) += value * t;
  }
};


struct GrainRepository : Array {
  /*
  Loads up a file and sorts grains by volume.
  */
  int grainSampleLength;
  vector<vector<float>> grainsByLoudness;

  GrainRepository(string filePath, float grainLength)
  : grainSampleLength(int(grainLength * SAMPLE_RATE))
  {
    load(filePath.c_str());
    generateSortedGrainsList();
  }

  void generateSortedGrainsList() {

    vector<pair<float, vector<float>>> rmsAndGrains;

    float sumOfSquares = 0;
    int samplesSummed = 0;
    float * startOfGrain = data();
    for(float& f : *this) {
      sumOfSquares += f * f;
      samplesSummed++;
      if (samplesSummed >= grainSampleLength) {
        // enough has been read to count as a grain
        float rms = pow(sumOfSquares, 0.5);
        vector<float> grain(startOfGrain, &f + 1);
        rmsAndGrains.push_back({rms, grain});
        sumOfSquares = 0;
        samplesSummed = 0;
        startOfGrain = &f + 1;
      }
    }

    sort(rmsAndGrains.begin(), rmsAndGrains.end());

    grainsByLoudness.clear();
    for(auto& rmsAndGrain : rmsAndGrains) {
      grainsByLoudness.push_back(rmsAndGrain.second);
    }
  }
};


struct StringRecombinator {
  vector<vector<vector<string>>> paragraphs;

  StringRecombinator(string inputString, string wordDelimiter = R"(\s+)",
    string sentenceDelimiter = R"([.?!])", string paragraphDelimiter = R"(\n)")
  {
    for(string paragraphString : resplit(inputString, paragraphDelimiter)) {
      vector<vector<string>> paragraph;
      for(string sentenceString : resplit(paragraphString, sentenceDelimiter)) {
        paragraph.push_back(resplit(sentenceString, wordDelimiter));
      }
      paragraphs.push_back(paragraph);
    }
  }

  string regenerate(float wordStretch, float sentenceStretch,
    float paragraphStretch, float documentStretch)
  {
    string out = "";
    for(int p = 0; p < (paragraphs.size() * documentStretch); ++p)
    {
      // go in order until you go through all the paragraphs, then choose randomly
      vector<vector<string>>& thisParagraph = (p < paragraphs.size()) ?
        paragraphs.at(p) : paragraphs.at(rand() % paragraphs.size());

      for(int s = 0; s < (thisParagraph.size() * paragraphStretch); ++s)
      {
        // go in order until you go through all the sentences, then choose randomly
        vector<string>& thisSentence = (s < thisParagraph.size()) ?
          thisParagraph.at(s) : thisParagraph.at(rand() % thisParagraph.size());

        for(int w = 0; w < (thisSentence.size() * sentenceStretch); ++w)
        {
          // go in order until you go through all the sentences, then choose randomly
          string& thisWord = (w < thisSentence.size()) ?
            thisSentence.at(w) : thisSentence.at(rand() % thisSentence.size());
          for(int c = 0; c < (thisWord.length() * wordStretch); ++c) {
            out += (c < thisWord.length()) ?
              thisWord.at(c) : thisWord.at(rand() % thisWord.length());
          }
        }
      }
    }
    return out;
  }
};


static int verbose_flag;

int main (int argc, char **argv)
{
  string soundFilePath = "";
  string textFilePath = "";
  string wordDelimiter = R"(\s+)";
  string sentenceDelimiter = R"([.?!])";
  string paragraphDelimiter = R"(\n)";
  float wordStretch=1, sentenceStretch=1, paragraphStretch=1, documentStretch=1;
  int c;

  while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"verbose", no_argument, &verbose_flag, 1},
          {"brief", no_argument, &verbose_flag, 0},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"text", required_argument, 0, 't'},
          {"wordDelimiter", required_argument, 0, 'W'},
          {"sentenceDelimiter", required_argument, 0, 'S'},
          {"paragraphDelimiter", required_argument, 0, 'P'},
          {"wordStretch", required_argument, 0, 'w'},
          {"sentenceStretch", required_argument, 0, 's'},
          {"paragraphStretch", required_argument, 0, 'p'},
          {"documentStretch", required_argument, 0, 'd'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "t:W:S:P:w:s:p:d:",
                       long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          break;

        case 't':
          textFilePath = optarg;
          break;

        case 'W':
          wordDelimiter = optarg;
          break;

        case 'S':
          sentenceDelimiter = optarg;
          break;

        case 'P':
          paragraphDelimiter = optarg;
          break;

        case 'w':
          wordStretch = stof(optarg);
          break;

        case 's':
          sentenceStretch = stof(optarg);
          break;

        case 'p':
          paragraphStretch = stof(optarg);
          break;

        case 'd':
          documentStretch = stof(optarg);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort();
        }
    }

  if (verbose_flag)
    puts ("verbose flag is set");

  /* Assign the command line argument to soundFilePath. */
  if (optind < argc) {
    soundFilePath = argv[optind];
  } else {
    std::cerr << "Sound file path argument required." << '\n';
    abort();
  }
  GrainRepository grains(soundFilePath, 0.05);

  string textInput, line;
  if(textFilePath.length() > 0) {
    /* We're reading the text from a file.*/
    ifstream inputFile (textFilePath);
    if (inputFile.is_open()) {
      while (getline(inputFile,line)) {
        textInput += (textInput.length() > 0) ? "\n" + line: line;
      }
      inputFile.close();
    } else {
      std::cerr <<"Unable to open file " << textFilePath.c_str() << '\n';
      abort();
    }
  } else {
    /* We're reading the text from the input.*/

    while (getline(cin, line)) {
      textInput += (textInput.length() > 0) ? "\n" + line: line;
    }
  }

  // for(string s : resplit(textInput, wordDelimiter))
  //   printf("%s\n", s.c_str());
  // printf("Sound Path: %s\n", soundFilePath.c_str());
  // printf("Text Input: %s\n", textInput.c_str());

  StringRecombinator inputRecombinator(textInput, wordDelimiter,
    sentenceDelimiter, paragraphDelimiter);

  printf("%s\n", inputRecombinator.regenerate(
    wordStretch, sentenceStretch, paragraphStretch, documentStretch).c_str());
  // for(auto& grain : grains.grainsByLoudness) {
  //   for(float sample : grain) {
  //     say(sample);
  //   }
  // }
  exit (0);
}
