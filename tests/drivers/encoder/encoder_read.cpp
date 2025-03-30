#include "encoder.h"
#include <iostream>
#include <thread>
#include <chrono>

void speedCallback_R(double speed) {
    std::cout << "Current speed: " << speed << " units/s" << std::endl;
}

void speedCallback_L(double speed) {
    std::cout << "Current speed: " << speed << " units/s" << std::endl;
}

int main() {
    try {
        //Encoder encoder_L("gpiochip0", 27, 26, speedCallback_L);
        Encoder encoder_R("gpiochip0", 20, 21, speedCallback_R);
        //encoder_L.begin();
        encoder_R.begin();
        
        while (true) {
            //encoder_L.update();
            encoder_R.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
