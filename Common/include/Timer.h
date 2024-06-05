#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <thread>

class Timer {
public:
    Timer() {
        start_time_ = std::chrono::steady_clock::now();
    }

   float getDelta() {
        auto end_time = std::chrono::steady_clock::now();
        return static_cast<float>
            (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_).count()) / 1000.f;
   }

    void reset() {
        start_time_ = std::chrono::steady_clock::now();
    }

    static void Block(float time) {
        auto t = std::chrono::milliseconds(static_cast<long long>(time * 1000));
        std::this_thread::sleep_for(t);
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
};

#endif //TIMER_H