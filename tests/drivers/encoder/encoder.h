#ifndef ENCODER_H
#define ENCODER_H

#include <functional>
#include <gpiod.h>
#include <chrono>

class Encoder {
public:
    using Callback = std::function<void(double)>;

    Encoder(const char* chipname, int pinA, int pinB, Callback callback);
    ~Encoder();
    void begin();
    void update();

private:
    const char* chipname_;
    int pinA_, pinB_;
    struct gpiod_chip* chip_;
    struct gpiod_line* lineA_;
    struct gpiod_line* lineB_;
    int lastState_;
    int position_;
    std::chrono::time_point<std::chrono::steady_clock> lastTime_;
    Callback callback_;
    static void ISRWrapper(int event, void* context);
};

#endif // ENCODER_H