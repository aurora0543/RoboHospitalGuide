#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace cv::face;
using namespace std;

int main() {
    // 加载人脸检测器
    CascadeClassifier face_cascade;
    face_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml");
    if (face_cascade.empty()) {
        cerr << "无法加载人脸分类器！" << endl;
        return -1;
    }

    // 构建训练数据
    vector<Mat> images;
    vector<int> labels;

    // 读取已知人脸（user1.jpg -> label 1）
    Mat img1 = imread("user1.jpg", IMREAD_GRAYSCALE);
    Mat img2 = imread("user2.jpg", IMREAD_GRAYSCALE);
    if (img1.empty() || img2.empty()) {
        cerr << "无法加载训练图像！" << endl;
        return -1;
    }

    images.push_back(img1);
    labels.push_back(1);

    images.push_back(img2);
    labels.push_back(2);

    // 创建并训练识别器
    Ptr<LBPHFaceRecognizer> recognizer = LBPHFaceRecognizer::create();
    recognizer->train(images, labels);
    cout << "训练完成。" << endl;

    // 读取测试图片（libcamera 拍摄的图像）
    Mat test_color = imread("capture.jpg");
    if (test_color.empty()) {
        cerr << "无法读取测试图像！" << endl;
        return -1;
    }

    Mat test_gray;
    cvtColor(test_color, test_gray, COLOR_BGR2GRAY);

    // 检测人脸
    vector<Rect> faces;
    face_cascade.detectMultiScale(test_gray, faces);

    for (const auto& face : faces) {
        Mat faceROI = test_gray(face);
        resize(faceROI, faceROI, Size(200, 200)); // LBPH 推荐尺寸

        int predicted_label = -1;
        double confidence = 0.0;
        recognizer->predict(faceROI, predicted_label, confidence);

        // 显示结果
        string name = (predicted_label == 1) ? "用户1" : (predicted_label == 2) ? "用户2" : "未知";
        stringstream ss;
        ss << name << " (" << confidence << ")";
        cout << "识别结果: " << ss.str() << endl;

        rectangle(test_color, face, Scalar(0, 255, 0), 2);
        putText(test_color, ss.str(), Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2);
    }

    imshow("识别结果", test_color);
    waitKey(0);
    return 0;
}