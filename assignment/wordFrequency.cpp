#include <algorithm>      // sort
#include <iostream>       // cin, cout, printf
#include <unordered_map>  // unordered_map
#include <vector>         // vector
#include <utility>
using namespace std;

// NOTE: I noticed that this still works if we use the signature
// bool sorterFunction (auto &i, auto &j)
// ... which is kind of cool
bool sorterFunction (pair<string, unsigned> &i, pair<string, unsigned> &j) {
  // we check if the first element has a *greater* count than the second
  // This way, it sorts from largest to smallest count
  return i.second > j.second;
}

int main() {
  unordered_map<string, unsigned> dictionary;

  string word;
  while (cin >> word) {
    // check if there's already an entry for word
    if (dictionary.count(word) > 0) {
      // if so, increment the counter for this word
      dictionary[word]++;
    } else {
      // otherwise, create the entry for this word and set its count to 1
      dictionary[word] = 1;
    }
  }

  vector<pair<string, unsigned>> wordList;
  for (auto& t : dictionary) wordList.push_back(t);

  sort(wordList.begin(), wordList.end(), sorterFunction);

  for (auto& t : wordList) printf("%u:%s\n", t.second, t.first.c_str());
}
