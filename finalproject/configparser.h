#ifndef _STRING_GRANULATOR_CONFIG_PARSER
#define _STRING_GRANULATOR_CONFIG_PARSER

#include <string>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <regex>
#include "utils/utils.h"
using namespace std;

static int verbose_flag;

struct StringGranulatorConfig {
  string soundFilePath = "";
  string textToGranulate = "";
  string wordDelimiter = R"(\s+)", sentenceDelimiter = R"([.?!])",
         paragraphDelimiter = R"(\n)";
  float wordStretch = 1, sentenceStretch = 1,
        paragraphStretch = 1, documentStretch = 1;
  int wordSpacing = 1, sentenceSpacing = 2, paragraphSpacing = 3;
  float contentGrainMinLoudness = 0.5, contentGrainMaxLoudness = 1.0;
  float wordDelimiterGrainLoudness = 0.3, sentenceDelimiterGrainLoudness = 0.2,
    paragraphDelimiterGrainLoudness = 0.1;
  bool deriveLetterFrequencyFromText = false;
  float grainLength = 0.1;
  string outputFileName = "out.wav";
};

void parseConfigFile(string configFilePath, StringGranulatorConfig &configObject) {
  string line;
  ifstream configfile (configFilePath);
  if (configfile.is_open())
  {
    while ( getline (configfile, line) )
    {
      regex configPattern(R"((\w+)\s*:\s*(.*))");
      auto line_begin = std::sregex_iterator(line.begin(), line.end(), configPattern);
      auto line_end = std::sregex_iterator();

      // Question: what the hell does ++i mean on such a weird object
      for(sregex_iterator i = line_begin; i != line_end; ++i) {
        smatch match = *i;
        string variableName = match.str(1);
        string variableValue = match.str(2);
        if (variableName == "wordDelimiter") {
          configObject.wordDelimiter = variableValue;
        } else if (variableName == "sentenceDelimiter") {
          configObject.sentenceDelimiter = variableValue;
        } else if (variableName == "paragraphDelimiter") {
          configObject.paragraphDelimiter = variableValue;
        } else if (variableName == "wordStretch") {
          configObject.wordStretch = atof(variableValue.c_str());
        } else if (variableName == "sentenceStretch") {
          configObject.sentenceStretch = atof(variableValue.c_str());
        } else if (variableName == "paragraphStretch") {
          configObject.paragraphStretch = atof(variableValue.c_str());
        } else if (variableName == "documentStretch") {
          configObject.documentStretch = atof(variableValue.c_str());
        } else if (variableName == "wordSpacing") {
          configObject.wordSpacing = atoi(variableValue.c_str());
        } else if (variableName == "sentenceSpacing") {
          configObject.sentenceSpacing = atoi(variableValue.c_str());
        } else if (variableName == "paragraphSpacing") {
          configObject.paragraphSpacing = atoi(variableValue.c_str());
        } else if (variableName == "contentGrainMinLoudness") {
          configObject.contentGrainMinLoudness = atof(variableValue.c_str());
        } else if (variableName == "contentGrainMaxLoudness") {
          configObject.contentGrainMaxLoudness = atof(variableValue.c_str());
        } else if (variableName == "wordDelimiterGrainLoudness") {
          configObject.wordDelimiterGrainLoudness = atof(variableValue.c_str());
        } else if (variableName == "sentenceDelimiterGrainLoudness") {
          configObject.sentenceDelimiterGrainLoudness = atof(variableValue.c_str());
        } else if (variableName == "paragraphDelimiterGrainLoudness") {
          configObject.paragraphDelimiterGrainLoudness = atof(variableValue.c_str());
        } else if (variableName == "deriveLetterFrequencyFromText") {
          configObject.deriveLetterFrequencyFromText = bool(atoi(variableValue.c_str()));
        } else if (variableName == "grainLength") {
          configObject.grainLength = atof(variableValue.c_str());
        } else if (variableName == "outputFileName") {
          configObject.outputFileName = variableValue;
        }
      }
    }
    configfile.close();
  }
}

StringGranulatorConfig parseConfigurations (int argc, char **argv)
{
  StringGranulatorConfig config;
  string textFilePath;
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
          {"config", required_argument, 0, 'c'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "t:c:",
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

        case 'c':
          parseConfigFile(optarg, config);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort();
        }
    }

  /* Assign the command line argument to soundFilePath. */
  if (optind < argc) {
    config.soundFilePath = argv[optind];
  } else {
    std::cerr << "Sound file path argument required." << '\n';
    abort();
  }

  string line;
  if(textFilePath.length() > 0) {
    /* We're reading the text from a file.*/
    ifstream inputFile (textFilePath);
    if (inputFile.is_open()) {
      while (getline(inputFile,line)) {
        config.textToGranulate += (config.textToGranulate.length() > 0) ? "\n" + line: line;
      }
      inputFile.close();
    } else {
      std::cerr <<"Unable to open file " << textFilePath.c_str() << '\n';
      abort();
    }
  } else {
    /* We're reading the text from the input.*/

    while (getline(cin, line)) {
      config.textToGranulate += (config.textToGranulate.length() > 0) ? "\n" + line: line;
    }
  }

  return config;
}

#endif
