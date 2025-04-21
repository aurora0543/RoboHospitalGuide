// maincontroller.h
#pragma once

#include <QString>
#include <QObject>
#include <memory>
#include <string>
#include "face_recognizer.h"
#include "json.hpp"

void playAudio(const std::string& path);

class MainController{
public:
    MainController();
    bool init();

    QString recognizeFace();
    void startNavigationTo(const QString& department);
    void exitSystem();

private:
    std::shared_ptr<FaceRecognizerLib> recognizer;
    nlohmann::json navJson;
    
    
};  