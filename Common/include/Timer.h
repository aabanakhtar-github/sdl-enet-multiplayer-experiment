#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <thread>

class Timer 
{
public:
    Timer() 
    {
        m_StartTime = m_Clock.now();
    }

    float GetDelta()
    {
        auto m_EndTime = m_Clock.now(); 
        return static_cast<float>
            (std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count()) / 1000.f; 
    }

    void Reset()
    {
        m_StartTime = m_Clock.now(); 
    }

    void Block(float time)
    {
        auto t = std::chrono::milliseconds(static_cast<long long>(time * 1000));
        std::this_thread::sleep_for(t);
    }
private:
    std::chrono::steady_clock m_Clock;
    std::chrono::time_point<decltype(m_Clock)> m_StartTime, m_EndTime;
};
#endif //TIMER_H