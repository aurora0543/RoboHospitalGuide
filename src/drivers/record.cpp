#include "record.h"
#include <iostream>
#include <cstdlib>

namespace Record {

bool recordToWav(const std::string& outputPath, int durationSec) {
    // 使用 arecord 命令录制 WAV 文件，采样率 16000Hz, 单声道, 16 位深度
    std::string command = "arecord -f cd -t wav -d " + std::to_string(durationSec) + " -r 16000 -c 1 " + outputPath;
    std::cout << "[INFO] Recording audio to: " << outputPath << " for " << durationSec << " seconds..." << std::endl;

    int ret = std::system(command.c_str());
    if (ret != 0) {
        std::cerr << "[ERROR] Failed to record audio." << std::endl;
        return false;
    }

    std::cout << "[INFO] Recording completed." << std::endl;
    return true;
}

bool recordWithSilenceDetection(const std::string& outputPath, int silenceThresholdDb, int silenceDurationSec) {
    std::string command =
    "sox -t alsa plughw:0,0 " + outputPath +
    " silence 1 0.1 " + std::to_string(silenceThresholdDb) + "d "
    "1 " + std::to_string(silenceDurationSec) + " " + std::to_string(silenceThresholdDb) + "d";

    std::cout << "[INFO] Recording with silence detection to: " << outputPath << std::endl;
    int ret = std::system(command.c_str());
    if (ret != 0) {
        std::cerr << "[ERROR] Failed to record audio with silence detection." << std::endl;
        return false;
    }
    std::cout << "[INFO] Recording finished (silence detected)." << std::endl;
    return true;
}


} // namespace AudioRecorder
