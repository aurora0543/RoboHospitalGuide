#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/dnn.h>
#include <dlib/opencv.h>
#include <dlib/image_io.h>
#include <iostream>
#include <vector>

// 定义深度神经网络
template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = dlib::add_prev1<block<N, BN, 1, dlib::tag1<SUBNET>>>;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = dlib::add_prev2<dlib::avg_pool<2, 2, 2, 2, dlib::skip1<dlib::tag2<block<N, BN, 2, dlib::tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block = BN<dlib::con<N, 3, 3, 1, 1, dlib::relu<BN<dlib::con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET> using ares = dlib::relu<residual<block, N, dlib::affine, SUBNET>>;
template <int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block, N, dlib::affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

using anet_type = dlib::loss_metric<dlib::fc_no_bias<128, dlib::avg_pool_everything<
    alevel0<
    alevel1<
    alevel2<
    alevel3<
    alevel4<
    dlib::max_pool<3, 3, 2, 2, dlib::relu<dlib::affine<dlib::con<32, 7, 7, 2, 2,
    dlib::input_rgb_image_sized<150>
    >>>>>>>>>>>;

class FaceRecognitionSystem {
private:
    dlib::frontal_face_detector detector;
    dlib::shape_predictor sp;
    anet_type net;
    std::vector<std::pair<std::string, dlib::matrix<float, 0, 1>>> known_faces;
    const float threshold = 0.5f;

public:
    FaceRecognitionSystem(const std::string& shape_predictor_path, const std::string& face_recognition_model_path) {
        // 加载人脸检测器
        detector = dlib::get_frontal_face_detector();
        
        // 加载人脸特征点预测器
        dlib::deserialize(shape_predictor_path) >> sp;
        
        // 加载人脸特征提取模型
        dlib::deserialize(face_recognition_model_path) >> net;
        
        std::cout << "模型加载完成。" << std::endl;
    }

    // 添加已知人脸
    void addKnownFace(const std::string& name, const std::string& image_path) {
        dlib::matrix<dlib::rgb_pixel> img;
        dlib::load_image(img, image_path);
        
        auto faces = detector(img);
        if (faces.size() == 0) {
            std::cout << "无法在图像中检测到人脸: " << image_path << std::endl;
            return;
        }
        
        auto face = faces[0];
        auto shape = sp(img, face);
        dlib::matrix<dlib::rgb_pixel> face_chip;
        dlib::extract_image_chip(img, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);
        
        dlib::matrix<float, 0, 1> face_descriptor = net(face_chip);
        known_faces.push_back(std::make_pair(name, face_descriptor));
        
        std::cout << "已添加 " << name << " 的人脸特征。" << std::endl;
    }

    // 计算两个人脸特征向量之间的相似度
    float calculateSimilarity(const dlib::matrix<float, 0, 1>& face1, const dlib::matrix<float, 0, 1>& face2) {
        return dlib::length(face1 - face2);
    }

    // 从摄像头识别人脸
    void recognizeFromCamera() {
        if (known_faces.empty()) {
            std::cout << "请先添加已知人脸。" << std::endl;
            return;
        }

        cv::VideoCapture cap(0);
        if (!cap.isOpened()) {
            std::cout << "无法打开摄像头。" << std::endl;
            return;
        }

        cv::Mat frame;
        dlib::matrix<dlib::rgb_pixel> img;
        
        while (true) {
            cap >> frame;
            if (frame.empty()) {
                std::cout << "无法捕获到图像。" << std::endl;
                break;
            }

            // 将OpenCV图像转换为dlib格式
            dlib::cv_image<dlib::bgr_pixel> dlib_frame(frame);
            dlib::matrix<dlib::rgb_pixel> dlib_rgb;
            dlib::assign_image(dlib_rgb, dlib_frame);
            
            // 检测人脸
            auto faces = detector(dlib_rgb);
            
            for (auto face : faces) {
                // 绘制人脸矩形
                cv::rectangle(frame, 
                              cv::Point(face.left(), face.top()), 
                              cv::Point(face.right(), face.bottom()), 
                              cv::Scalar(0, 255, 0), 2);
                
                auto shape = sp(dlib_rgb, face);
                dlib::matrix<dlib::rgb_pixel> face_chip;
                dlib::extract_image_chip(dlib_rgb, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);
                
                // 提取人脸特征
                dlib::matrix<float, 0, 1> face_descriptor = net(face_chip);
                
                // 与已知人脸进行比对
                std::string best_match_name = "未知";
                float best_similarity = 1.0;
                float confidence = 0.0;
                
                for (const auto& known_face : known_faces) {
                    float similarity = calculateSimilarity(known_face.second, face_descriptor);
                    if (similarity < best_similarity) {
                        best_similarity = similarity;
                        best_match_name = known_face.first;
                        // 将相似度转换为置信度 (距离越小，置信度越高)
                        confidence = 1.0f - similarity;
                    }
                }
                
                // 判断是否符合要求
                std::string result = (confidence > threshold) ? "符合" : "不符合";
                
                // 在图像上显示结果
                std::stringstream ss;
                ss << best_match_name << ": " << confidence * 100 << "% (" << result << ")";
                cv::putText(frame, ss.str(), 
                            cv::Point(face.left(), face.top() - 10), 
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, 
                            (confidence > threshold) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 
                            2);
            }
            
            // 显示结果
            cv::imshow("人脸识别", frame);
            
            // 按ESC键退出
            if (cv::waitKey(1) == 27) {
                break;
            }
        }
        
        cap.release();
        cv::destroyAllWindows();
    }
};

int main() {
    try {
        // 请提供模型文件的路径
        std::string shape_predictor_path = "shape_predictor_68_face_landmarks.dat";
        std::string face_recognition_model_path = "dlib_face_recognition_resnet_model_v1.dat";
        
        FaceRecognitionSystem system(shape_predictor_path, face_recognition_model_path);
        
        // 添加已知人脸
        std::cout << "正在添加已知人脸..." << std::endl;
        system.addKnownFace("用户1", "user1.jpg");
        // 可以添加更多已知人脸
        // system.addKnownFace("用户2", "user2.jpg");
        
        // 从摄像头开始识别
        std::cout << "启动摄像头识别..." << std::endl;
        system.recognizeFromCamera();
        
    } catch (const std::exception& e) {
        std::cout << "发生错误: " << e.what() << std::endl;
    }
    
    return 0;
}

