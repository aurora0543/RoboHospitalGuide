// maincontroller.cpp
#include "maincontroller.h"
#include "nav.h"
#include "motor.h"
#include "servo.h"
//#include "servonew.h"
#include "yaw_tracker.h"
#include <fstream>
#include <thread>
#include "face_recognizer.h"
#include "json.hpp"

void playAudio(const std::string& path) {
    std::string cmd = "mplayer -ao alsa:device=hw=1.0 -volume 100 \"" + path + "\" > /dev/null 2>&1";
    system(cmd.c_str());
}


MainController::MainController() : recognizer(std::make_shared<FaceRecognizerLib>()) {}

bool MainController::init() {

    // Initialize face recognizer
    std::string face_folder = "../source/face";

    if (!recognizer->init(face_folder)) {
        std::cerr << "[DEBUG] Failed to initialize face recognition.\n";
        return false;
    }

    // Initialize Motor, Servo, and YawTracker

    return true;
}

QString MainController::recognizeFace() {
    // std::string result = recognizer->recognize("../source/tmp/capture.jpg");
    // return QString::fromStdString(result);
    return QString("None"); // Placeholder for actual recognition
}

void MainController::startNavigationTo(const QString& departmentName) {
 
    std::string audio_start = "../source/starts.mp3";
    std::string audio_stop  = "../source/stops.mp3";

    // Load navigation JSON
    nlohmann::json navJson;
    std::ifstream navFile("../config/nav.json");
    if (!navFile.is_open()) {
        std::cerr << "[DEBUG] Failed to open navigation config file.\n";
        return;
    }
    navFile >> navJson;
    navFile.close();

    std::string department = departmentName.trimmed().toStdString();
    if (!navJson.contains(department)) {
        std::cerr << "[DEBUG] Department not found: " << department << "\n";
        return;
    }
    
    MotorPins pins = {17, 16, 22, 23};
    auto motor = std::make_shared<Motor>(pins);
    auto servo = std::make_shared<Servo>(18);
    auto yaw   = std::make_shared<YawTracker>();
    std::thread([motor, servo, yaw, department, navData = navJson, audio_start, audio_stop]() {
        playAudio(audio_start);
        Nav::startNavigation.store(true);
        Nav::pauseNavigation.store(false);
        Nav::navCV.notify_all();

        Nav::navigationThread(motor.get(), servo.get(), yaw.get(), department, navData);

        playAudio(audio_stop);
    }).detach();
}