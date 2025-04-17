#include "MainThread.h"
#include <thread> 

#include "record.h"

int main() {
//    Record::recordWithSilenceDetection("../source/speak.wav", -35, 2);
    std::thread cmdThread(startMainThread);
    cmdThread.join();
    return 0;
}