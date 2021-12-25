#pragma once

#include "IOsAbstraction.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>
#include <map>

class OsAbstraction
{
public:
    OsAbstraction();
    void SetOutputInterface(IOsAbstraction::Output* iOutput);
    void StartTimer(int timerId, int timerPeriodInMilliSeconds);
    void StopTimer(int timerId);
    void Trace(const char* tag, const char* text, va_list arguments);

private:
    IOsAbstraction::Output* _iOutput = nullptr;
    std::map<int, TimerHandle_t> _timerHandleById;
    class CommonHandling
    {
    public:
        CommonHandling(SemaphoreHandle_t& processingMutex);
        void Trace(const char* tag, const char* text, va_list arguments);
        static void TimerCallback(TimerHandle_t timerHandle);

        std::map<TimerHandle_t, int> _timerIdByHandle;
        
    private:
        const SemaphoreHandle_t& _processingMutex;
        char _traceBuffer[1000];
    };
    static CommonHandling _commonHandling;
};