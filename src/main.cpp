#include "MainThread.h"
#include <thread> 


int main() {
    std::thread cmdThread(startMainThread);
    cmdThread.join();
    return 0;
}