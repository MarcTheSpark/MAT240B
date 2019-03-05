#include <cmath>
#include <iostream>
using namespace std;

int main(int argc, char const *argv[]) {
  for(float i=0; i < 50; i++) {
    cout << sin((long double)((1e16 + 0.1 * i) * M_PI_2)) << endl;
  }
  return 0;
}
