#ifndef _GRAIN_REPOSITORY
#define _GRAIN_REPOSITORY

#include "utils/utils.h"
#include <vector>
#include <string>
#include <cmath>
using namespace diy;
using namespace std;

// From synths.h
struct Array : std::vector<float> {
  void operator()(float f) {
    push_back(f);
    //
  }
  void save(const char* fileName) const {
    drwav_data_format format;
    format.channels = 1;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.sampleRate = 44100;
    format.bitsPerSample = 32;
    drwav* pWav = drwav_open_file_write(fileName, &format);
    if (pWav == nullptr) {
      std::cerr << "failed to write " << fileName << std::endl;
      drwav_close(pWav);
      return;
    }
    drwav_uint64 samplesWritten = drwav_write(pWav, size(), data());
    if (samplesWritten != size()) {
      std::cerr << "failed to write all samples to " << fileName << std::endl;
      drwav_close(pWav);
      return;
    }
    drwav_close(pWav);
  }

  bool load(const char* fileName) {
    drwav* pWav = drwav_open_file(fileName);
    if (pWav == nullptr) return false;

    if (pWav->channels == 1) {
      resize(pWav->totalPCMFrameCount);
      drwav_read_f32(pWav, size(), data());  // XXX does fail?
      drwav_close(pWav);
      return true;
    }

    if (pWav->channels > 1) {
      float* pSampleData = (float*)malloc((size_t)pWav->totalPCMFrameCount *
                                          pWav->channels * sizeof(float));
      drwav_read_f32(pWav, pWav->totalPCMFrameCount, pSampleData);
      drwav_close(pWav);

      resize(pWav->totalPCMFrameCount);
      for (unsigned i = 0; i < size(); ++i)
        at(i) = pSampleData[pWav->channels * i];  // only read the first channel
      return true;
    }

    return false;
  }

  // raw lookup
  // except that i think "at" does bounds checking
  float raw(const float index) const {
    const unsigned i = floor(index);
    const float x0 = at(i);
    const float x1 = at((i == (size() - 1)) ? 0 : i + 1);  // looping semantics
    const float t = index - i;
    return x1 * t + x0 * (1 - t);
  }

  // void resize(unsigned n) { data.resize(n, 0); }
  // float& operator[](unsigned index) { return data[index]; }

  // allow for sloppy indexing (e.g., negative, huge) by fixing the index to
  // within the bounds of the array
  float get(float index) const {
    if (index < 0) index += size();
    if (index > size()) index -= size();
    return raw(index);  // defer to our method without bounds checking
  }
  float operator[](const float index) const { return get(index); }
  float phasor(float index) const { return get(size() * index); }

  void add(const float index, const float value) {
    const unsigned i = floor(index);
    const unsigned j = (i == (size() - 1)) ? 0 : i + 1;  // looping semantics
    const float t = index - i;
    at(i) += value * (1 - t);
    at(j) += value * t;
  }
};


struct GrainRepository : Array {
  /*
  Loads up a file and sorts grains by volume.
  */
  int grainSampleLength;
  vector<vector<float>> grainsByLoudness;

  GrainRepository(string filePath, float grainLength)
  : grainSampleLength(int(grainLength * SAMPLE_RATE))
  {
    load(filePath.c_str());
    generateSortedGrainsList();
  }

  void generateSortedGrainsList() {
    vector<pair<float, vector<float>>> rmsAndGrains;
    float sumOfSquares = 0;
    int samplesSummed = 0;
    float * startOfGrain = data();
    for(float& f : *this) {
      sumOfSquares += f * f;
      samplesSummed++;
      if (samplesSummed >= grainSampleLength) {
        // enough has been read to count as a grain
        float rms = pow(sumOfSquares, 0.5);
        vector<float> grain(startOfGrain, &f + 1);
        rmsAndGrains.push_back({rms, grain});
        sumOfSquares = 0;
        samplesSummed = 0;
        startOfGrain = &f + 1;
      }
    }

    sort(rmsAndGrains.begin(), rmsAndGrains.end());

    grainsByLoudness.clear();
    for(auto& rmsAndGrain : rmsAndGrains) {
      grainsByLoudness.push_back(rmsAndGrain.second);
    }
  }

  vector<float> getWindowedGrain(float normalizedLoudness) {
    vector<float> out;
    vector<float>& unwindowedGrain = grainsByLoudness.at(
      int(normalizedLoudness * (grainsByLoudness.size() - 1))
    );

    for(int i=0; i < grainSampleLength; ++i) {
      out.push_back(unwindowedGrain[i] * sin(i * M_PI / grainSampleLength));
    }
    return out;
  }
};

#endif
