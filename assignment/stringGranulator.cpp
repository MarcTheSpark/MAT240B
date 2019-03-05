#include "../../unix/everything.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
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
          {"wordDelimiter", required_argument, 0, 'w'},
          {"sentenceDelimiter", required_argument, 0, 's'},
          {"paragraphDelimiter", required_argument, 0, 'p'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "t:w:s:p:",
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

        case 'w':
          wordDelimiter = optarg;
          break;

        case 's':
          sentenceDelimiter = optarg;
          break;

        case 'p':
          paragraphDelimiter = optarg;
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

  printf("%s\n", inputRecombinator.regenerate(1, 2, 1, 1).c_str());
  exit (0);
}
