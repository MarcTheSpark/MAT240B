rm stringGranulator out.wav
c++ --std=c++11 stringGranulator.cpp -o stringGranulator
./stringGranulator WeirdPoem.wav -t "stringToGranulate.txt"
# xdg-open out.wav
