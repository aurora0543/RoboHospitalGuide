#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <vector>
#include <map>

class FaceRecognizerLib {
public:
    // 初始化并加载人脸数据
    bool init(const std::string& face_folder);

    // 识别给定图片中的人脸，返回最相似的人脸图片名
    std::pair<std::string, double> recognize(const std::string& capture_image_path);

private:
    cv::CascadeClassifier face_cascade;
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer;
    std::map<int, std::string> label_to_name;
    int current_label = 0;

    void loadFacesFromFolder(const std::string& folder);
    std::string getFaceFileNameFromLabel(int label);
};

#endif // FACE_RECOGNIZER_H