#include "OsAbstraction.h"
#include <esp_log.h>
#include <stdio.h>

namespace PlatformSpecific
{
    OsAbstraction::OsAbstraction(SemaphoreHandle_t& processingMutex) :
        _processingMutex(processingMutex)
    {

    }

    PlatformSpecific::IOsAbstraction::Input::TimerHandle OsAbstraction::IpsOsAbstractionStartTimer(int timerId, int timerPeriodInMilliSeconds, PlatformSpecific::IOsAbstraction::Output* iOutput)
    {
        TimerHandle_t freeRtosTimerHandle = xTimerCreate(
            "OsAbstraction",
            pdMS_TO_TICKS(timerPeriodInMilliSeconds),
            pdFALSE,
            this,
            TimerCallback
        );
        PlatformSpecific::IOsAbstraction::Input::TimerHandle interfaceTimerHandle = _currentInterfaceTimerHandle++;
        _timerInfoByfreeRtosTimerHandle[freeRtosTimerHandle] = std::make_tuple(interfaceTimerHandle, timerId, iOutput);
        _freeRtosTimerHandleByInterfaceTimerHandle[interfaceTimerHandle] = freeRtosTimerHandle;
        xTimerStart(freeRtosTimerHandle, 0);
        return interfaceTimerHandle;
    }

    void OsAbstraction::IpsOsAbstractionStopTimer(PlatformSpecific::IOsAbstraction::Input::TimerHandle timerHandle)
    {
        auto freeRtosTimerHandleByInterfaceTimerHandleIterator = _freeRtosTimerHandleByInterfaceTimerHandle.find(timerHandle);
        if (freeRtosTimerHandleByInterfaceTimerHandleIterator != _freeRtosTimerHandleByInterfaceTimerHandle.end())
        {
            // Although we call xTimerDelete the callback can still be called
            // Because we erase the handle from the map the callback will not act
            xTimerDelete(freeRtosTimerHandleByInterfaceTimerHandleIterator->second, 0);
            _timerInfoByfreeRtosTimerHandle.erase(freeRtosTimerHandleByInterfaceTimerHandleIterator->second);
            _freeRtosTimerHandleByInterfaceTimerHandle.erase(timerHandle);
        }
    }

    void OsAbstraction::IpsOsAbstractionTrace(const char* tag, const char* text, va_list arguments)
    {
        vsnprintf(_traceBuffer, sizeof(_traceBuffer), text, arguments);
        ESP_LOGI(tag, "%s", _traceBuffer);
    }

    void OsAbstraction::TimerCallback(TimerHandle_t freeRtosTimerHandle)
    {
        // =============================
        // = TO PREVENT STACK OVERFLOW =
        // =============================
        // The stack size has to be set to 4096 in the IDF menuconfig, under:
        // Component config -> FreeRTOS -> FreeRTOS timer task stack size
        //
        // The pointer to the OsAbstraction instance is stored in the freeRTOS timer's ID
        // We will use the timer handle to identify the timer
        // The handle is unique from the moment the timer is created until the moment freeRTOS releases it
        // In case StopTimer already called xTimerDelete it is still possible that the callback is called,
        // however because freeRTOS has not yet released the handle we can safely use it to check the
        // _timerInfoByfreeRtosTimerHandle map (and see it is not present there anymore)
        OsAbstraction* osAbstraction = (OsAbstraction*)pvTimerGetTimerID(freeRtosTimerHandle);
        xSemaphoreTake(osAbstraction->_processingMutex, portMAX_DELAY);
        auto timerInfoByfreeRtosTimerHandleIterator = osAbstraction->_timerInfoByfreeRtosTimerHandle.find(freeRtosTimerHandle);
        if (timerInfoByfreeRtosTimerHandleIterator != osAbstraction->_timerInfoByfreeRtosTimerHandle.end())
        {
            // Be sure to store the info because the iterator will be destroyed by the erase function
            int timerId = std::get<1>(timerInfoByfreeRtosTimerHandleIterator->second);
            IOsAbstraction::Output* outputInterface = std::get<2>(timerInfoByfreeRtosTimerHandleIterator->second);
            // Cleanup first before calling the function to handle the timer expiry
            // If the handling function starts the timer again we will not inadvertedly clean it up
            xTimerDelete(freeRtosTimerHandle, 0);
            osAbstraction->_timerInfoByfreeRtosTimerHandle.erase(freeRtosTimerHandle);
            osAbstraction->_freeRtosTimerHandleByInterfaceTimerHandle.erase(std::get<0>(timerInfoByfreeRtosTimerHandleIterator->second));
            // Finally call the handling function
            outputInterface->IpsOsAbstractionHandleExpiredTimer(timerId);
        }
        xSemaphoreGive(osAbstraction->_processingMutex);
    }
}