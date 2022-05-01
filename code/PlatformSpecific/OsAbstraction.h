#pragma once

#include "IOsAbstraction.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>
#include <map>

namespace PlatformSpecific
{
    class OsAbstraction : public PlatformSpecific::IOsAbstraction::Input
    {
    public:
        OsAbstraction(SemaphoreHandle_t& processingMutex);

    private:
        // PlatformSpecific::IOsAbstraction::Input
        PlatformSpecific::IOsAbstraction::Input::TimerHandle IpsOsAbstractionStartTimer(int timerId, int timerPeriodInMilliSeconds, PlatformSpecific::IOsAbstraction::Output* iOutput) override;
        void IpsOsAbstractionStopTimer(PlatformSpecific::IOsAbstraction::Input::TimerHandle timerHandle) override;
        void IpsOsAbstractionTrace(const char* tag, const char* text, va_list arguments) override;

        static void TimerCallback(TimerHandle_t freeRtosTimerHandle);

        const SemaphoreHandle_t& _processingMutex;
        PlatformSpecific::IOsAbstraction::Input::TimerHandle _currentInterfaceTimerHandle = 0;
        std::map<TimerHandle_t, std::tuple<PlatformSpecific::IOsAbstraction::Input::TimerHandle, int, PlatformSpecific::IOsAbstraction::Output*>> _timerInfoByfreeRtosTimerHandle;
        std::map<int, TimerHandle_t> _freeRtosTimerHandleByInterfaceTimerHandle;
        char _traceBuffer[1000];
    };
}