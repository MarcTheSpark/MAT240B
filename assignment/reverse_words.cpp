#include <iostream>
#include <string>

using namespace std;

int main() {
  while (true) {
    printf("Type a sentence (then hit return): ");
    string line;
    getline(cin, line);
    if (!cin.good() || line.length() == 0) {
      printf("Done\n");
      return 0;
    }
    string out = "";
    string reversedWord= "";
    for(int i = line.length() - 1; i >= 0; --i) {
      if (line[i] == ' ') {
        if (reversedWord.length() > 0) {
          if (out.length() > 0) out = reversedWord + ' ' + out;
          else out = reversedWord;
        }
        reversedWord = "";
      } else {
        reversedWord += line[i];
      }
    }
    if (reversedWord.length() > 0) {
      if (out.length() > 0) out = reversedWord + ' ' + out;
      else out = reversedWord;
    }
    printf("%s\n", out.c_str());
  }
}
