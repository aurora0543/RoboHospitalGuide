#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat image = cv::imread("test.jpg");
    if (image.empty()) {
        std::cerr << "无法加载图像！请确认 test.jpg 是否存在于当前目录。" << std::endl;
        return -1;
    }
    cv::imshow("Test Image", image);
    cv::waitKey(0);
    return 0;
}