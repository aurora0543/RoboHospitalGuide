/**
 * 医疗导诊机器人 - 人脸识别模块
 * 开发平台: 树莓派5 (Linux)
 * 功能: 通过摄像头捕获图像，识别病人身份，并提供导诊建议
 */

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <mysql/mysql.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

// 使用命名空间
using namespace std;
using namespace cv;
using json = nlohmann::json;

// 定义病人信息结构体
struct PatientInfo {
    int id;                     // 病人ID
    string name;                // 姓名
    string gender;              // 性别
    int age;                    // 年龄
    string department;          // 就诊科室
    string doctorName;          // 医生姓名
    string appointmentTime;     // 预约时间
    string faceImagePath;       // 人脸图像路径
};

// 日志级别
enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

// 全局变量
mutex mtx;                      // 互斥锁，用于线程同步
condition_variable cv;          // 条件变量，用于线程通信
queue<Mat> frameQueue;          // 帧队列，存储待处理的视频帧
bool exitProgram = false;       // 退出程序标志
const int MAX_QUEUE_SIZE = 10;  // 最大队列大小
const string CONFIG_FILE = "/home/pi/medical_robot/config.json"; // 配置文件路径

// 配置参数
struct Config {
    // 数据库配置
    string dbHost;
    string dbUser;
    string dbPassword;
    string dbName;
    int dbPort;
    
    // 摄像头配置
    int cameraId;
    int frameWidth;
    int frameHeight;
    int fps;
    
    // 人脸识别配置
    double recognitionThreshold;
    string faceDatasetPath;
    string modelPath;
    
    // 日志配置
    string logPath;
    LogLevel logLevel;
};

// 全局配置对象
Config config;

/**
 * 日志类 - 用于记录系统运行日志
 */
class Logger {
private:
    string logFilePath;
    LogLevel level;
    mutex logMutex;
    ofstream logFile;
    
public:
    Logger(const string& path, LogLevel lvl) : logFilePath(path), level(lvl) {
        logFile.open(logFilePath, ios::app);
        if (!logFile.is_open()) {
            cerr << "无法打开日志文件: " << logFilePath << endl;
        }
    }
    
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void log(LogLevel lvl, const string& message) {
        if (lvl < level) return;
        
        lock_guard<mutex> lock(logMutex);
        
        // 获取当前时间
        auto now = chrono::system_clock::now();
        time_t currentTime = chrono::system_clock::to_time_t(now);
        struct tm* timeInfo = localtime(&currentTime);
        char timeBuffer[80];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);
        
        // 日志级别转字符串
        string levelStr;
        switch (lvl) {
            case DEBUG:   levelStr = "DEBUG"; break;
            case INFO:    levelStr = "INFO"; break;
            case WARNING: levelStr = "WARNING"; break;
            case ERROR:   levelStr = "ERROR"; break;
            default:      levelStr = "UNKNOWN";
        }
        
        // 写入日志
        string logEntry = string(timeBuffer) + " [" + levelStr + "] " + message;
        
        if (logFile.is_open()) {
            logFile << logEntry << endl;
        }
        
        // 同时输出到控制台
        cout << logEntry << endl;
    }
};

// 全局日志对象
unique_ptr<Logger> logger;

/**
 * 数据库管理类 - 负责与医院数据库的交互
 */
class DatabaseManager {
private:
    MYSQL* conn;
    string host;
    string user;
    string password;
    string dbname;
    int port;
    
public:
    DatabaseManager(const string& h, const string& u, const string& pwd, const string& db, int p)
        : host(h), user(u), password(pwd), dbname(db), port(p), conn(nullptr) {}
    
    ~DatabaseManager() {
        disconnect();
    }
    
    bool connect() {
        conn = mysql_init(nullptr);
        if (conn == nullptr) {
            logger->log(ERROR, "MySQL初始化失败");
            return false;
        }
        
        if (mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), 
                              dbname.c_str(), port, nullptr, 0) == nullptr) {
            logger->log(ERROR, "数据库连接失败: " + string(mysql_error(conn)));
            mysql_close(conn);
            conn = nullptr;
            return false;
        }
        
        // 设置字符集为UTF8，以支持中文
        mysql_set_character_set(conn, "utf8");
        
        logger->log(INFO, "数据库连接成功");
        return true;
    }
    
    void disconnect() {
        if (conn != nullptr) {
            mysql_close(conn);
            conn = nullptr;
            logger->log(INFO, "数据库连接已关闭");
        }
    }
    
    vector<PatientInfo> getAllPatients() {
        vector<PatientInfo> patients;
        
        if (conn == nullptr) {
            logger->log(ERROR, "获取病人信息前数据库未连接");
            return patients;
        }
        
        const char* query = "SELECT id, name, gender, age, department, doctor_name, "
                           "appointment_time, face_image_path FROM patients";
        
        if (mysql_query(conn, query) != 0) {
            logger->log(ERROR, "查询病人信息失败: " + string(mysql_error(conn)));
            return patients;
        }
        
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr) {
            logger->log(ERROR, "获取查询结果失败: " + string(mysql_error(conn)));
            return patients;
        }
        
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            PatientInfo patient;
            patient.id = atoi(row[0]);
            patient.name = row[1] ? row[1] : "";
            patient.gender = row[2] ? row[2] : "";
            patient.age = row[3] ? atoi(row[3]) : 0;
            patient.department = row[4] ? row[4] : "";
            patient.doctorName = row[5] ? row[5] : "";
            patient.appointmentTime = row[6] ? row[6] : "";
            patient.faceImagePath = row[7] ? row[7] : "";
            
            patients.push_back(patient);
        }
        
        mysql_free_result(result);
        logger->log(INFO, "成功获取 " + to_string(patients.size()) + " 条病人记录");
        
        return patients;
    }
    
    PatientInfo getPatientById(int id) {
        PatientInfo patient;
        patient.id = -1; // 默认为无效ID
        
        if (conn == nullptr) {
            logger->log(ERROR, "获取病人信息前数据库未连接");
            return patient;
        }
        
        string query = "SELECT id, name, gender, age, department, doctor_name, "
                      "appointment_time, face_image_path FROM patients WHERE id = " + to_string(id);
        
        if (mysql_query(conn, query.c_str()) != 0) {
            logger->log(ERROR, "查询病人信息失败: " + string(mysql_error(conn)));
            return patient;
        }
        
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr) {
            logger->log(ERROR, "获取查询结果失败: " + string(mysql_error(conn)));
            return patient;
        }
        
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            patient.id = atoi(row[0]);
            patient.name = row[1] ? row[1] : "";
            patient.gender = row[2] ? row[2] : "";
            patient.age = row[3] ? atoi(row[3]) : 0;
            patient.department = row[4] ? row[4] : "";
            patient.doctorName = row[5] ? row[5] : "";
            patient.appointmentTime = row[6] ? row[6] : "";
            patient.faceImagePath = row[7] ? row[7] : "";
        }
        
        mysql_free_result(result);
        return patient;
    }
};

/**
 * 人脸识别类 - 负责人脸检测和识别
 */
class FaceRecognizer {
private:
    CascadeClassifier faceDetector;       // OpenCV级联分类器，用于人脸检测
    Ptr<cv::face::LBPHFaceRecognizer> model; // LBPH人脸识别模型
    vector<PatientInfo> patients;         // 已注册的病人列表
    double confidenceThreshold;           // 识别置信度阈值
    
public:
    FaceRecognizer(double threshold) : confidenceThreshold(threshold) {
        // 初始化人脸检测器
        if (!faceDetector.load("/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_alt2.xml")) {
            logger->log(ERROR, "无法加载人脸检测模型");
            throw runtime_error("无法加载人脸检测模型");
        }
        
        // 初始化人脸识别模型
        model = cv::face::LBPHFaceRecognizer::create();
        
        logger->log(INFO, "人脸识别模块初始化完成");
    }
    
    bool loadModel(const string& modelPath) {
        try {
            model->read(modelPath);
            logger->log(INFO, "成功加载人脸识别模型: " + modelPath);
            return true;
        } catch (const exception& e) {
            logger->log(ERROR, "加载人脸识别模型失败: " + string(e.what()));
            return false;
        }
    }
    
    bool trainModel(const vector<PatientInfo>& patientData, const string& modelOutput) {
        if (patientData.empty()) {
            logger->log(ERROR, "训练数据为空");
            return false;
        }
        
        vector<Mat> images;
        vector<int> labels;
        
        // 收集训练数据
        for (const auto& patient : patientData) {
            // 检查患者是否有面部图像
            if (patient.faceImagePath.empty()) {
                continue;
            }
            
            Mat faceImage = imread(patient.faceImagePath, IMREAD_GRAYSCALE);
            if (faceImage.empty()) {
                logger->log(WARNING, "无法读取患者 " + to_string(patient.id) + 
                          " 的面部图像: " + patient.faceImagePath);
                continue;
            }
            
            // 检测面部区域
            vector<Rect> faces;
            faceDetector.detectMultiScale(faceImage, faces);
            
            if (faces.empty()) {
                logger->log(WARNING, "在患者 " + to_string(patient.id) + 
                          " 的图像中未检测到面部");
                continue;
            }
            
            // 裁剪并调整面部区域大小
            Mat faceROI = faceImage(faces[0]);
            Mat resizedFace;
            resize(faceROI, resizedFace, Size(100, 100));
            
            // 添加到训练数据
            images.push_back(resizedFace);
            labels.push_back(patient.id);
        }
        
        if (images.empty()) {
            logger->log(ERROR, "没有可用的面部图像进行训练");
            return false;
        }
        
        // 训练模型
        logger->log(INFO, "开始训练人脸识别模型，使用 " + to_string(images.size()) + " 张图像");
        model->train(images, labels);
        
        // 保存模型
        try {
            model->save(modelOutput);
            logger->log(INFO, "人脸识别模型训练完成并保存到: " + modelOutput);
            
            // 保存训练后的病人列表以便后续识别
            patients = patientData;
            
            return true;
        } catch (const exception& e) {
            logger->log(ERROR, "保存人脸识别模型失败: " + string(e.what()));
            return false;
        }
    }
    
    pair<bool, PatientInfo> recognizeFace(const Mat& frame) {
        // 创建结果对象
        PatientInfo emptyPatient;
        emptyPatient.id = -1;
        
        // 转换为灰度图像
        Mat grayFrame;
        cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
        
        // 检测人脸
        vector<Rect> faces;
        faceDetector.detectMultiScale(grayFrame, faces, 1.1, 5, 0, Size(30, 30));
        
        if (faces.empty()) {
            return {false, emptyPatient};
        }
        
        // 对检测到的最大人脸进行识别
        Rect* largestFace = &faces[0];
        for (size_t i = 1; i < faces.size(); i++) {
            if (faces[i].area() > largestFace->area()) {
                largestFace = &faces[i];
            }
        }
        
        // 提取人脸区域并调整大小
        Mat faceROI = grayFrame(*largestFace);
        Mat resizedFace;
        resize(faceROI, resizedFace, Size(100, 100));
        
        // 使用模型预测
        int predictedLabel = -1;
        double confidence = 0.0;
        model->predict(resizedFace, predictedLabel, confidence);
        
        // 标记识别结果
        rectangle(frame, *largestFace, Scalar(0, 255, 0), 2);
        
        // 如果置信度低于阈值，认为识别成功
        if (confidence < confidenceThreshold) {
            // 查找匹配的病人
            for (const auto& patient : patients) {
                if (patient.id == predictedLabel) {
                    // 在图像上显示识别结果
                    string text = "ID: " + to_string(patient.id) + ", " + patient.name;
                    putText(frame, text, Point(largestFace->x, largestFace->y - 10),
                            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
                    
                    logger->log(INFO, "识别到病人: " + patient.name + 
                              ", 置信度: " + to_string(confidence));
                    
                    return {true, patient};
                }
            }
        } else {
            // 在图像上显示未识别
            putText(frame, "Unknown", Point(largestFace->x, largestFace->y - 10),
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
        }
        
        return {false, emptyPatient};
    }
    
    void setPatients(const vector<PatientInfo>& patientList) {
        patients = patientList;
    }
};

/**
 * 摄像头管理类 - 负责操作摄像头捕获视频
 */
class CameraManager {
private:
    VideoCapture camera;
    int deviceId;
    int frameWidth;
    int frameHeight;
    int frameRate;
    bool isRunning;
    thread captureThread;
    
public:
    CameraManager(int id, int width, int height, int fps)
        : deviceId(id), frameWidth(width), frameHeight(height), frameRate(fps), isRunning(false) {}
    
    ~CameraManager() {
        stop();
    }
    
    bool init() {
        camera.open(deviceId);
        if (!camera.isOpened()) {
            logger->log(ERROR, "无法打开摄像头 ID: " + to_string(deviceId));
            return false;
        }
        
        // 设置摄像头参数
        camera.set(CAP_PROP_FRAME_WIDTH, frameWidth);
        camera.set(CAP_PROP_FRAME_HEIGHT, frameHeight);
        camera.set(CAP_PROP_FPS, frameRate);
        
        // 获取实际设置的参数
        int actualWidth = camera.get(CAP_PROP_FRAME_WIDTH);
        int actualHeight = camera.get(CAP_PROP_FRAME_HEIGHT);
        int actualFPS = camera.get(CAP_PROP_FPS);
        
        logger->log(INFO, "摄像头初始化成功：分辨率 " + to_string(actualWidth) + "x" + 
                  to_string(actualHeight) + "，帧率 " + to_string(actualFPS));
        
        return true;
    }
    
    void start() {
        if (isRunning) return;
        
        isRunning = true;
        captureThread = thread(&CameraManager::captureLoop, this);
        logger->log(INFO, "摄像头捕获线程已启动");
    }
    
    void stop() {
        if (!isRunning) return;
        
        isRunning = false;
        
        // 唤醒所有等待线程
        cv.notify_all();
        
        if (captureThread.joinable()) {
            captureThread.join();
        }
        
        if (camera.isOpened()) {
            camera.release();
        }
        
        logger->log(INFO, "摄像头捕获线程已停止");
    }
    
private:
    void captureLoop() {
        while (isRunning) {
            Mat frame;
            if (!camera.read(frame)) {
                logger->log(WARNING, "无法从摄像头读取帧");
                this_thread::sleep_for(chrono::milliseconds(100));
                continue;
            }
            
            {
                unique_lock<mutex> lock(mtx);
                
                // 如果队列已满，移除最旧的帧
                while (frameQueue.size() >= MAX_QUEUE_SIZE) {
                    frameQueue.pop();
                }
                
                frameQueue.push(frame);
                
                // 通知处理线程有新帧可用
                cv.notify_one();
            }
            
            // 控制捕获速率
            this_thread::sleep_for(chrono::milliseconds(1000 / frameRate));
        }
    }
};

/**
 * 主控类 - 协调各个模块工作
 */
class MedicalRobot {
private:
    unique_ptr<DatabaseManager> dbManager;
    unique_ptr<FaceRecognizer> faceRecognizer;
    unique_ptr<CameraManager> cameraManager;
    thread processingThread;
    bool isRunning;
    
public:
    MedicalRobot() : isRunning(false) {}
    
    ~MedicalRobot() {
        stop();
    }
    
    bool init() {
        // 读取配置文件
        if (!loadConfig()) {
            return false;
        }
        
        // 初始化日志
        logger = make_unique<Logger>(config.logPath, config.logLevel);
        logger->log(INFO, "医疗导诊机器人启动中...");
        
        // 初始化数据库连接
        dbManager = make_unique<DatabaseManager>(
            config.dbHost, config.dbUser, config.dbPassword, config.dbName, config.dbPort);
        
        if (!dbManager->connect()) {
            logger->log(ERROR, "数据库连接失败");
            return false;
        }
        
        // 获取病人数据
        vector<PatientInfo> patients = dbManager->getAllPatients();
        if (patients.empty()) {
            logger->log(WARNING, "数据库中没有病人信息");
        } else {
            logger->log(INFO, "从数据库加载了 " + to_string(patients.size()) + " 条病人记录");
        }
        
        // 初始化人脸识别器
        try {
            faceRecognizer = make_unique<FaceRecognizer>(config.recognitionThreshold);
            
            // 尝试加载现有模型
            bool modelLoaded = faceRecognizer->loadModel(config.modelPath);
            
            // 如果没有现有模型，训练一个新的
            if (!modelLoaded && !patients.empty()) {
                logger->log(INFO, "未找到现有模型，开始训练新模型");
                faceRecognizer->trainModel(patients, config.modelPath);
            }
            
            // 设置病人列表
            faceRecognizer->setPatients(patients);
            
        } catch (const exception& e) {
            logger->log(ERROR, "初始化人脸识别器失败: " + string(e.what()));
            return false;
        }
        
        // 初始化摄像头
        cameraManager = make_unique<CameraManager>(
            config.cameraId, config.frameWidth, config.frameHeight, config.fps);
        
        if (!cameraManager->init()) {
            logger->log(ERROR, "摄像头初始化失败");
            return false;
        }
        
        logger->log(INFO, "医疗导诊机器人初始化完成");
        return true;
    }
    
    void start() {
        if (isRunning) return;
        
        isRunning = true;
        
        // 启动摄像头
        cameraManager->start();
        
        // 启动处理线程
        processingThread = thread(&MedicalRobot::processFrames, this);
        
        logger->log(INFO, "医疗导诊机器人系统已启动");
    }
    
    void stop() {
        if (!isRunning) return;
        
        isRunning = false;
        exitProgram = true;
        
        // 停止摄像头
        cameraManager->stop();
        
        // 唤醒处理线程
        cv.notify_all();
        
        // 等待处理线程结束
        if (processingThread.joinable()) {
            processingThread.join();
        }
        
        logger->log(INFO, "医疗导诊机器人系统已停止");
    }
    
private:
    bool loadConfig() {
        try {
            ifstream configFile(CONFIG_FILE);
            if (!configFile.is_open()) {
                cerr << "无法打开配置文件: " << CONFIG_FILE << endl;
                return false;
            }
            
            json configJson;
            configFile >> configJson;
            
            // 数据库配置
            config.dbHost = configJson["database"]["host"];
            config.dbUser = configJson["database"]["user"];
            config.dbPassword = configJson["database"]["password"];
            config.dbName = configJson["database"]["name"];
            config.dbPort = configJson["database"]["port"];
            
            // 摄像头配置
            config.cameraId = configJson["camera"]["id"];
            config.frameWidth = configJson["camera"]["width"];
            config.frameHeight = configJson["camera"]["height"];
            config.fps = configJson["camera"]["fps"];
            
            // 人脸识别配置
            config.recognitionThreshold = configJson["face_recognition"]["threshold"];
            config.faceDatasetPath = configJson["face_recognition"]["dataset_path"];
            config.modelPath = configJson["face_recognition"]["model_path"];
            
            // 日志配置
            config.logPath = configJson["logger"]["path"];
            string logLevelStr = configJson["logger"]["level"];
            
            if (logLevelStr == "DEBUG") {
                config.logLevel = DEBUG;
            } else if (logLevelStr == "INFO") {
                config.logLevel = INFO;
            } else if (logLevelStr == "WARNING") {
                config.logLevel = WARNING;
            } else if (logLevelStr == "ERROR") {
                config.logLevel = ERROR;
            } else {
                config.logLevel = INFO;  // 默认为INFO级别
            }
            
            return true;
        } catch (const exception& e) {
            cerr << "读取配置文件失败: " << e.what() << endl;
            return false;
        }
    }
    
    void processFrames() {
        logger->log(INFO, "帧处理线程已启动");
        
        // 创建窗口显示
        namedWindow("医疗导诊机器人", WINDOW_NORMAL);
        resizeWindow("医疗导诊机器人", config.frameWidth, config.frameHeight);
        
        while (isRunning) {
            Mat frame;
            
            {
                unique_lock<mutex> lock(mtx);
                
                // 等待新帧或退出信号
                cv.wait(lock, []{ return !frameQueue.empty() || exitProgram; });
                
                if (exitProgram) {
                    break;
                }
                
                if (frameQueue.empty()) {
                    continue;
                }
                
                frame = frameQueue.front();
                frameQueue.pop();
            }
            
            // 进行人脸识别
            auto [recognized, patient] = faceRecognizer->recognizeFace(frame);
            
            // 如果识别成功，显示病人信息
            if (recognized) {
                // 在帧上显示病人信息
                string info = "患者: " + patient.name + ", " + patient.gender + ", " + to_string(patient.age) + "岁";
                string dept = "科室: " + patient.department + ", 医生: " + patient.doctorName;
                string appt = "预约时间: " + patient.appointmentTime;
                
                int baseline = 0;
                Size textSize = getTextSize(info, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
                
                rectangle(frame, Point(0, frame.rows - 80), 
                         Point(frame.cols, frame.rows), Scalar(0, 0, 0), FILLED);
                
                putText(frame, info, Point(10, frame.rows - 60), 
                       FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
                putText(frame, dept, Point(10, frame.rows - 40), 
                       FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
                putText(frame, appt, Point(10, frame.rows - 20), 
                       FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
                
                // 记录识别结果
                logger->log(INFO, "成功识别患者: " + patient.name + 
                          ", 导诊至: " + patient.department);
            }
            
            // 显示结果
            imshow("医疗导诊机器人", frame);
            
            // 检查键盘输入
            int key = waitKey(1);
            if (key == 27) {  // ESC键
                break;
            }
        }
        
        destroyAllWindows();
        logger->log(INFO, "帧处理线程已结束");
    }
};

/**
 * 主函数
 */
int main(int argc, char** argv) {
    MedicalRobot robot;
    
    if (!robot.init()) {
        cerr << "初始化医疗导诊机器人失败" << endl;
        return -1;
    }
    
    robot.start();
    
    // 等待用户输入退出
    cout << "医疗导诊机器人正在运行..." << endl;
    cout << "按 ESC 键或 Ctrl+C 退出程序" << endl;
    
    // 程序运行直到主处理线程结束
    while (!exitProgram) {
        this_thread::sleep_for(chrono::seconds(1));
        
        // 捕获控制台输入
        if (cin.get() == 'q') {
            exitProgram = true;
        }
    }
    
    // 停止机器人
    robot.stop();
    
    cout << "医疗导诊机器人已停止" << endl;
    return 0;
}
