#pragma once

#include "IOsAbstraction.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>
#include <list>
#include <tuple>
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
        static void TimerHandlerTask(void* pvParameters);
        void TimerHandlerTask();

        const SemaphoreHandle_t& _processingMutex;
        TaskHandle_t _timerHandlerTask = {};
        std::list <PlatformSpecific::IOsAbstraction::Input::TimerHandle> _timerHandlerTaskExpiredTimersList;
        PlatformSpecific::IOsAbstraction::Input::TimerHandle _currentInterfaceTimerHandle = 0;
        std::map<PlatformSpecific::IOsAbstraction::Input::TimerHandle, std::tuple<int, PlatformSpecific::IOsAbstraction::Output*, TimerHandle_t>> _timerInfoByInterfaceTimerHandle;
        std::map<TimerHandle_t, PlatformSpecific::IOsAbstraction::Input::TimerHandle> _interfaceTimerHandleByFreeRtosTimerHandle;
        char _traceBuffer[1000];
    };
}