#include "utils.h"

void writeWaveform(vector<float> samples, string filePath) {
  {
    drwav_data_format format;
    format.channels = 1;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.sampleRate = 44100;
    format.bitsPerSample = 32;
    drwav* pWav = drwav_open_file_write(filePath.c_str(), &format);
    drwav_uint64 samplesWritten = drwav_write(pWav, samples.size(), &samples[0]);
    drwav_close(pWav);
    assert(samplesWritten == samples.size());
  }
}
