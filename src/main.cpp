#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>
#include "motor.h"
#include "servo.h"
#include "yaw_tracker.h"
#include "nav.h"
#include "json.hpp"  // nlohmann::json 的头文件

// 播放音频文件的简单函数（需要 mplayer 工具安装）
void playAudio(const std::string& audioFile) {
    std::string command = "mplayer -volume 100 " + audioFile + " >/dev/null 2>&1";
    //std::clog << "执行命令: " << command << std::endl; // 输出调试信息
    int ret = system(command.c_str());
    if (ret != 0) {
        std::cerr << "❌ 播放失败: " << audioFile << std::endl;
    }
}

int main() {
    MotorPins pins = { 17, 16, 22, 23 };
    std::string audio_start = "../source/starts.mp3";
    std::string audio_hold = "../source/hold.mp3";
    std::string audio_stop = "../source/stops.mp3";

    try {
        Motor motor(pins);
        Servo servo(18);
        YawTracker yaw;

        // 在 main 中读取导航 JSON 配置文件
        nlohmann::json navJson;
        std::ifstream navFile("../config/nav.json");
        if (navFile.is_open()) {
            navFile >> navJson;
            navFile.close();
        } else {
            std::cerr << "❌ 无法打开导航配置文件\n";
            return 1;
        }

        // 启动导航线程，将读取到的 navJson 传递给 navigationThread
        std::thread navThread(Nav::navigationThread, &motor, &servo, &yaw, navJson);

        // 使用调试输出命令自动控制导航，不占用终端
        std::clog << "调试输出：启动导航\n";
        playAudio(audio_start);
        Nav::startNavigation.store(true);
        Nav::pauseNavigation.store(false);
        Nav::navCV.notify_all();

        // 等待 5 秒以让导航动作开始
        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::clog << "调试输出：暂停导航\n";
        playAudio(audio_hold);
        Nav::pauseNavigation.store(true);

        // 等待 2 秒
        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::clog << "调试输出：恢复导航\n";
        playAudio(audio_start);
        Nav::pauseNavigation.store(false);
        Nav::navCV.notify_all();

        // 等待 5 秒后退出程序
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::clog << "调试输出：退出程序\n";
        playAudio(audio_stop);

        navThread.detach();  // 或根据实际需求选择 join()

    } catch (const std::exception& ex) {
        std::cerr << "❌ 错误: " << ex.what() << std::endl;
    }
    
    return 0;
}