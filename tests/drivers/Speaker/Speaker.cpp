#include <cstdlib>
#include <string>
#include <iostream>

int main() {
    // 指定要播放的音频文件路径
    std::string audioFilePath = "Free_Test_Data_100KB_MP3.mp3";

    // 构建命令行命令
    std::string command = "mplayer " + audioFilePath;

    // 使用system函数执行命令
    int result = system(command.c_str());

    // 检查命令是否成功执行
    if (result == 0) {
        std::cout << "音频播放成功！" << std::endl;
    } else {
        std::cerr << "音频播放失败！" << std::endl;
    }

    return 0;
}