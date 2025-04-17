#ifndef RECORD_H
#define RECORD_H
#include <string>


namespace Record {
    bool recordToWav(const std::string& outputPath, int durationSec);
    bool recordWithSilenceDetection(const std::string& outputPath, int silenceThresholdDb, int silenceDurationSec);
}

#endif