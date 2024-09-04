#include "Os.h"

namespace PlatformIndependent::Commons
{
    Os::Timer* Os::Timer::_lastTimer = nullptr;

    Os::Os()
    {
        _iOs = nullptr;
    }

    void Os::SetOutputInterface(PlatformIndependent::Commons::IOs::Output* iOs)
    {
        _iOs = iOs;
    }

    void Os::StartTimer(int timerId, int timerPeriod)
    {
        Timer* timer = Timer::FindTimer(timerId);
        if (timer->_running)
        {
            _iOs->IPiOsStopTimer(timerId);
        }
        else
        {
            timer->_running = true;
        }

        Trace("Starting timer %s with a period of %d ms", timer->_name, timerPeriod);
        _iOs->IPiOsStartTimer(timerId, timerPeriod);
    }

    void Os::StopTimer(int timerId)
    {
        Timer* timer = Timer::FindTimer(timerId);
        if (timer->_running)
        {
            Trace("Stopping timer %s", timer->_name);
            _iOs->IPiOsStopTimer(timerId);
            timer->_running = false;
        }
    }

    bool Os::TimerRunning(int timerId)
    {
        return Timer::FindTimer(timerId)->_running;
    }

    void Os::Trace(const char* text, ...)
    {
        va_list arguments;
        va_start(arguments, text);
        _iOs->IPiOsTrace(text, arguments);
        va_end(arguments);
    }

    void Os::IPiOsHandleExpiredTimer(int timerId)
    {
        Timer* timer = Timer::FindTimer(timerId);
        timer->_running = false;
        HandleExpiredTimer(timerId, timer->_name);
    }

    Os::Timer::Timer(int timerId, const char* name) :
        _timerId(timerId),
        _name(name),
        _previousTimer(_lastTimer)
    {
        _running = false;
        _lastTimer = this;
    }

    Os::Timer* Os::Timer::FindTimer(int timerId)
    {
        Timer* timer = Timer::_lastTimer;
        do
        {
            if (timer->_timerId == timerId)
            {
                return timer;
            }
            timer = timer->_previousTimer;
        } while (timer != nullptr);
        return nullptr;
    }
}