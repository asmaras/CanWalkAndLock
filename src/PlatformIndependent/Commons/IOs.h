#pragma once

#include <stdarg.h>

namespace PlatformIndependent::Commons
{
    class IOs
    {
    public:
        class Input
        {
        public:
            // Function calls to input interfaces should always be mutually excluded
            virtual void IPiOsHandleExpiredTimer(int timerId) = 0;
        };

        class Output
        {
        public:
            virtual void IPiOsStartTimer(int timerId, int timerPeriodInMilliSeconds) = 0;
            virtual void IPiOsStopTimer(int timerId) = 0;
            virtual void IPiOsTrace(const char* text, va_list arguments) = 0;
        };
    };
}