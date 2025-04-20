#include "face_recognizer.h"
#include <filesystem> 

bool FaceRecognizerLib::init(const std::string& face_folder) {
    if (!face_cascade.load("/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "无法加载人脸分类器" << std::endl;
        return false;
    }
    recognizer = cv::face::LBPHFaceRecognizer::create();
    loadFacesFromFolder(face_folder);
    return true;
}

void FaceRecognizerLib::loadFacesFromFolder(const std::string& folder) {
    if (!std::filesystem::is_directory(folder)) {
        std::cerr << "❌ 提供的路径不是有效的目录: " << folder << std::endl;
        return;
    }

    std::vector<cv::Mat> images;
    std::vector<int> labels;

    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::string fullpath = entry.path().string();

        cv::Mat img = cv::imread(fullpath, cv::IMREAD_GRAYSCALE);
        if (img.empty()) {
            std::cerr << "⚠️ 无法读取图像: " << fullpath << std::endl;
            continue;
        }

        cv::resize(img, img, cv::Size(200, 200));
        images.push_back(img);
        labels.push_back(current_label);
        label_to_name[current_label] = filename;
        current_label++;
    }

    if (!images.empty()) {
        recognizer->train(images, labels);
        std::cout << "✅ 已加载 " << images.size() << " 张训练图像\n";
    } else {
        std::cerr << "⚠️ 没有可用的人脸图像用于训练\n";
    }
}

// std::string FaceRecognizerLib::recognize(const std::string& capture_image_path) {
//     cv::Mat img_color = cv::imread(capture_image_path);
//     if (img_color.empty()) {
//         std::cerr << "无法读取图像：" << capture_image_path << std::endl;
//         return "";
//     }

//     cv::Mat img_gray;
//     cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);

//     std::vector<cv::Rect> faces;
//     face_cascade.detectMultiScale(img_gray, faces);

//     int best_label = -1;
//     double best_confidence = 1000.0;

//     for (const auto& face : faces) {
//         cv::Mat faceROI = img_gray(face);
//         cv::resize(faceROI, faceROI, cv::Size(200, 200));

//         int predicted_label = -1;
//         double confidence = 0.0;
//         recognizer->predict(faceROI, predicted_label, confidence);

//         if (confidence < best_confidence) {
//             best_confidence = confidence;
//             best_label = predicted_label;
//         }
//     }

//     if (best_label != -1) {
//         return getFaceFileNameFromLabel(best_label);
//     } else {
//         return "未知";
//     }
// }

std::pair<std::string, double> FaceRecognizerLib::recognize(const std::string& capture_image_path) {
    cv::Mat img_color = cv::imread(capture_image_path);
    if (img_color.empty()) {
        std::cerr << "无法读取图像：" << capture_image_path << std::endl;
        return {"未知", -1.0};
    }

    cv::Mat img_gray;
    cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(img_gray, faces);

    int best_label = -1;
    double best_confidence = 1000.0;

    for (const auto& face : faces) {
        cv::Mat faceROI = img_gray(face);
        cv::resize(faceROI, faceROI, cv::Size(200, 200));

        int predicted_label = -1;
        double confidence = 0.0;
        recognizer->predict(faceROI, predicted_label, confidence);

        if (confidence < best_confidence) {
            best_confidence = confidence;
            best_label = predicted_label;
        }
    }

    if (best_label != -1) {
        std::string filename = getFaceFileNameFromLabel(best_label);
        return {filename, best_confidence};
    } else {
        return {"未知", -1.0};
    }
}

std::string FaceRecognizerLib::getFaceFileNameFromLabel(int label) {
    if (label_to_name.count(label)) {
        return label_to_name[label];
    }
    return "未知";
}