#include "MainThread.h"
#include "motor.h"
#include "servo.h"
#include "yaw_tracker.h"
#include "nav.h"
#include "face_recognizer.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <memory>
#include <cstdlib>

void playAudio(const std::string& path) {
    std::string cmd = "mplayer -volume 100 " + path + " >/dev/null 2>&1";
    system(cmd.c_str());
}

void startMainThread() {
    std::string face_folder   = "../source/face";
    std::string capture_path  = "../source/tmp/capture.jpg";
    std::string audio_start   = "../source/starts.mp3";
    std::string audio_hold    = "../source/hold.mp3";
    std::string audio_stop    = "../source/stops.mp3";

    FaceRecognizerLib recognizer;
    if (!recognizer.init(face_folder)) {
        std::cerr << "[DEBUG] 人脸识别初始化失败\n";
    }

    while (true) {
        std::string command;
        std::cout << "请输入指令（如 nav 开始导航，facedetection 人脸识别）: ";
        std::cin >> command;

        if (command == "facedetection") {
            std::string result = recognizer.recognize(capture_path);
            std::cout << "识别结果：" << result << std::endl;
        }
        else if (command == "nav") {
            std::string department;
            std::cout << "请输入目标科室名称: ";
            std::cin.ignore();
            std::getline(std::cin, department);

            nlohmann::json navJson;
            std::ifstream navFile("../config/nav.json");
            if (!navFile.is_open()) {
                std::cerr << "[DEBUG] 无法打开导航配置文件\n";
                continue;
            }

            try {
                navFile >> navJson;
                navFile.close();

                if (!navJson.contains(department)) {
                    std::cerr << "[DEBUG] 找不到目标科室: " << department << "\n";
                    continue;
                }

                MotorPins pins = {17, 16, 22, 23};
                auto motor = std::make_shared<Motor>(pins);
                auto servo = std::make_shared<Servo>(18);
                auto yaw   = std::make_shared<YawTracker>();

                std::thread([motor, servo, yaw, department, navJson, audio_start, audio_stop]() {
                    playAudio(audio_start);
                    Nav::startNavigation.store(true);
                    Nav::pauseNavigation.store(false);
                    Nav::navCV.notify_all();

                    Nav::navigationThread(motor.get(), servo.get(), yaw.get(), department, navJson);

                    playAudio(audio_stop);
                }).detach();

            } catch (const std::exception& e) {
                std::cerr << "[DEBUG] 导航启动失败: " << e.what() << "\n";
                continue;
            }
        } else {
            std::cout << "[DEBUG] 未知指令: " << command << std::endl;
        }
    }
}
