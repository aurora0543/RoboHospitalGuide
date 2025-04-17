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
        std::cerr << "[DEBUG] Failed to initialize face recognition.\n";
    }

    while (true) {
        std::string command;
        std::cout << "Enter a command (nav, facedetection, help, list, voice, translate): ";
        std::cin >> command;

        if (command == "facedetection") {
            std::string result = recognizer.recognize(capture_path);
            std::cout << "Face recognition result: " << result << std::endl;
        }
        else if (command == "nav") {
            std::string department;
            std::cout << "Enter destination department: ";
            std::cin.ignore();
            std::getline(std::cin, department);

            nlohmann::json navJson;
            std::ifstream navFile("../config/nav.json");
            if (!navFile.is_open()) {
                std::cerr << "[DEBUG] Failed to open navigation config file.\n";
                continue;
            }

            try {
                navFile >> navJson;
                navFile.close();

                if (!navJson.contains(department)) {
                    std::cerr << "[DEBUG] Department not found: " << department << "\n";
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
                std::cerr << "[DEBUG] Navigation startup error: " << e.what() << "\n";
                continue;
            }
        }
        else if (command == "help") {
            std::cout << "[INFO] Available commands: nav, facedetection, help, list, voice, translate\n";
        }
        else if (command == "list") {
            std::cout << "[INFO] List feature not implemented yet.\n";
        }
        else if (command == "voice") {
            std::cout << "[INFO] Voice recognition feature not implemented yet.\n";
        }
        else if (command == "translate") {
            std::cout << "[INFO] Translation feature not implemented yet.\n";
        }
        else if (command == "quit") {
            std::cout << "[INFO] Quitting system..." << std::endl;
            break;
        }
        else {
            std::cout << "[DEBUG] Unknown command: " << command << std::endl;
        }
    }
}