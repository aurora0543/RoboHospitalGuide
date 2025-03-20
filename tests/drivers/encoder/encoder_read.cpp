#include "encoder.h"
#include <iostream>
#include <thread>
#include <chrono>

void speedCallback(double speed) {
    std::cout << "Current speed: " << speed << " units/s" << std::endl;
}

int main() {
    try {
        Encoder encoder("gpiochip0", 20, 21, speedCallback);
        encoder.begin();
        
        while (true) {
            encoder.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
