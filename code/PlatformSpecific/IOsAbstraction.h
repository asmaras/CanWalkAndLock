#pragma once

#include <stdarg.h>

namespace PlatformSpecific
{
    class IOsAbstraction
    {
    public:
        class Output
        {
        public:
            virtual void IpsOsAbstractionHandleExpiredTimer(int timerId) = 0;
        };

        class Input
        {
        public:
            using TimerHandle = int;
            virtual PlatformSpecific::IOsAbstraction::Input::TimerHandle IpsOsAbstractionStartTimer(int timerId, int timerPeriodInMilliSeconds, PlatformSpecific::IOsAbstraction::Output* iOutput) = 0;
            virtual void IpsOsAbstractionStopTimer(PlatformSpecific::IOsAbstraction::Input::TimerHandle timerHandle) = 0;
            virtual void IpsOsAbstractionTrace(const char* tag, const char* text, va_list arguments) = 0;
        };
    };
}