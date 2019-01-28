#include <regex> // regex
#include <iostream>

using namespace std;

int main() {
  string text, line;
  getline(cin, text);
  while (getline(cin, line)) {
    text += "\n" + line;
  }

  // Omg regex are so ugly
  regex word_regex(R"(<a .*?href=\"(.*?)\".*?>(.*?)</a>)");
  auto words_begin = std::sregex_iterator(text.begin(), text.end(), word_regex);
  auto words_end = std::sregex_iterator();

  // Question: what the hell does ++i mean on such a weird object
  for(sregex_iterator i = words_begin; i != words_end; ++i) {
    smatch match = *i;
    string target = match.str(1);
    string name = match.str(2);
    cout << name << " --> " << target << endl;
  }
}
