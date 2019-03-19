#ifndef _STRING_RECOMBINATOR
#define _STRING_RECOMBINATOR

#include <vector>
#include <string>

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
          for(int _=0; _ < 4; _++) out += char(-1);
        }
        for(int _=0; _ < 6; _++) out += char(-2);
      }
      for(int _=0; _ < 8; _++) out += char(-3);
    }
    return out;
  }
};

#endif
