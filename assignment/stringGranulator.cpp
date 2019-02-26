#include "everything.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace diy;
using namespace std;


static int verbose_flag;

int main (int argc, char **argv)
{
  string soundFilePath = "";
  string textFilePath = "";
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
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "t:",
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
  }

  string textInput, line;
  if(textFilePath.length() > 0) {
    /* We're reading the text from a file.*/
    ifstream inputFile (textFilePath);
    if (inputFile.is_open()) {
      while (getline(inputFile,line)) {
        if (!line.empty()) {
          textInput += (textInput.length() > 0) ? "\n" + line: line;
        } else {
          break;
        }
      }
      inputFile.close();
    } else {
      printf("Unable to open file %s\n", textFilePath.c_str());
      abort();
    }
  } else {
    /* We're reading the text from the input.*/

    while (getline(cin, line)) {
      if (!line.empty()) {
        textInput += (textInput.length() > 0) ? "\n" + line: line;
      } else {
        break;
      }
    }
  }

  printf("Sound Path: %s\n", soundFilePath.c_str());
  printf("Text Input: %s\n", textInput.c_str());

  exit (0);
}
