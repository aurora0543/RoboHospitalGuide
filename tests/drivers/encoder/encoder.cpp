#include "encoder.h"
#include <iostream>
#include <thread>

Encoder::Encoder(const char* chipname, int pinA, int pinB, Callback callback)
    : chipname_(chipname), pinA_(pinA), pinB_(pinB), callback_(callback), position_(0), lastState_(0) {
    chip_ = gpiod_chip_open_by_name(chipname_);
    if (!chip_) {
        throw std::runtime_error("Failed to open GPIO chip");
    }
    
    lineA_ = gpiod_chip_get_line(chip_, pinA_);
    lineB_ = gpiod_chip_get_line(chip_, pinB_);
    if (!lineA_ || !lineB_) {
        gpiod_chip_close(chip_);
        throw std::runtime_error("Failed to get GPIO lines");
    }
}

Encoder::~Encoder() {
    gpiod_chip_close(chip_);
}

void Encoder::begin() {
    if (gpiod_line_request_falling_edge_events(lineA_, "encoder") < 0 ||
        gpiod_line_request_falling_edge_events(lineB_, "encoder") < 0) {
        throw std::runtime_error("Failed to request GPIO events");
    }
    lastTime_ = std::chrono::steady_clock::now();
}

void Encoder::update() {
    struct gpiod_line_event event;
    while (gpiod_line_event_read(lineA_, &event) == 0 || gpiod_line_event_read(lineB_, &event) == 0) {
        int stateA = gpiod_line_get_value(lineA_);
        int stateB = gpiod_line_get_value(lineB_);
        int currentState = (stateA << 1) | stateB;
        
        auto now = std::chrono::steady_clock::now();
        double timeDiff = std::chrono::duration<double>(now - lastTime_).count();
        lastTime_ = now;
        
        if ((lastState_ == 0b00 && currentState == 0b01) ||
            (lastState_ == 0b01 && currentState == 0b11) ||
            (lastState_ == 0b11 && currentState == 0b10) ||
            (lastState_ == 0b10 && currentState == 0b00)) {
            position_++;
        } else if ((lastState_ == 0b00 && currentState == 0b10) ||
                   (lastState_ == 0b10 && currentState == 0b11) ||
                   (lastState_ == 0b11 && currentState == 0b01) ||
                   (lastState_ == 0b01 && currentState == 0b00)) {
            position_--;
        }
        lastState_ = currentState;
        
        double speed = 1.0 / timeDiff; // 简单速度计算
        callback_(speed);
    }
}
