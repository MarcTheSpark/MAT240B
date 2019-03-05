rm stringGranulator ../../unix/output.wav
c++ --std=c++11 stringGranulator.cpp -o stringGranulator
./stringGranulator AnamnesisReadingShort.ogg -t "stringToGranulate.txt"
# xdg-open ../unix/output.wav
