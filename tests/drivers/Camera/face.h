#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>
#include <map>

class FaceRecognitionSystem {
private:
    cv::CascadeClassifier faceDetector;
    cv::dnn::Net faceNet;          // 人脸检测网络
    cv::dnn::Net facerec;          // 人脸特征提取网络
    std::map<int, std::string> labelNames;
    std::map<int, std::vector<float>> faceDatabase; // 存储已知人脸特征
    
    const float cosineThreshold = 0.363f; // 余弦距离阈值 (~0.95相似度)
    const float l2Threshold = 1.128f;     // 欧氏距离阈值
    
    const std::string faceCascadePath = "haarcascade_frontalface_default.xml";
    const std::string faceNetModelPath = "face_detection_yunet.onnx";
    const std::string facerecModelPath = "face_recognition_sface.onnx";
    const std::string sourceDir = "source/face/";
    const std::string tmpDir = "tmp/";
    const std::string dbPath = "face_database.yml";
    
    // 预处理图像用于识别
    cv::Mat alignFace(const cv::Mat& frame, cv::Rect face);
    
    // 捕获图像（树莓派拍照或使用摄像头）
    cv::Mat captureImage(bool useRaspberryPi = true);
    
    // 使用树莓派相机拍照
    cv::Mat captureWithRaspberryPi();
    
    // 提取人脸特征
    std::vector<float> extractFaceFeature(const cv::Mat& face);
    
    // 计算相似度
    float calculateSimilarity(const std::vector<float>& feature1, const std::vector<float>& feature2);
    
    // 使用DNN进行人脸检测
    std::vector<cv::Rect> detectFacesDNN(const cv::Mat& frame);

public:
    FaceRecognitionSystem();
    ~FaceRecognitionSystem();
    
    // 加载模型和人脸数据库
    bool loadModelsAndDatabase();
    
    // 从目录加载训练图像
    bool buildFaceDatabase();
    
    // 保存人脸数据库
    bool saveFaceDatabase();
    
    // 加载人脸数据库
    bool loadFaceDatabase();
    
    // 添加人脸到数据库
    bool addFaceToDatabase(const std::string& name, int label, const cv::Mat& faceImage);
    
    // 识别单张图像中的人脸
    std::vector<std::pair<int, float>> recognizeFace(const cv::Mat& image);
    
    // 从摄像头识别人脸
    void recognizeFromCamera(bool useRaspberryPi = true);
    
    // 识别拍摄的照片
    std::vector<std::pair<int, float>> recognizeFromCapture(bool useRaspberryPi = true);
    
    // 获取指定标签的名称
    std::string getLabelName(int label) const;
};
