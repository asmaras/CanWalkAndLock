#include "OsAbstraction.h"
#ifdef USE_ESP32_FRAMEWORK_ARDUINO
// Enable ESP_LOGX macros for logging when built with ESPHome
#include "esphome/core/log.h"
using namespace esphome;
#else
// Note: to enable ESP_LOGX macros for logging when built with PlatformIO, add build flag -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_DEBUG
#include <esp_log.h>
#endif
#include <stdio.h>

// About the timers
// Timers are a bit more complicated than necessary because:
// - We want to pass a timer handle to the user that's independent of the freeRTOS TimerHandle_t.
// - FreeRtos can call the timer callback function even after xTimerDelete is called, if the timer was just about to fire. So even
//   if our application stops a timer it can still fire causing unexpected behaviour.
// - The freeRTOS timer task has a limited stack size resulting in stack overflow exceptions in application code called by the
//   timer callback function.
// We will use an incrementing integer as the "interface timer handle" to pass to the user. It will be valid from the call to
// IpsOsAbstractionStartTimer until IpsOsAbstractionStopTimer or until the user's timer expiry interface function is called. The
// timer info is put in a map indexed by the interface timer handle.
// We insert the interface timer handle in a second map indexed by the freeRTOS timer handle and use a semaphore to access the
// map and freeRTOS timer functions. When the user calls IpsOsAbstractionStopTimer, the handle is erased from the map so in the
// event the timer callback is called we can see the handle is not present anymore. Then we don't call
// IpsOsAbstractionHandleExpiredTimer.
// To solve the stack size problem we would normally increase the timer task stack size. This can unfortunately not be changed
// easily. Instead we offload the work of calling the user's interface function to a seperate task called TimerHandlerTask. This
// introduces an interval (between the freeRTOS timer callback function and the task actually handling the timer) in which the
// user could call IpsOsAbstractionStopTimer. However because we pass the interface timer handle of the expired timer to the
// TimerHandlerTask, we can simply check if the handle is stil present in the map and decide whether or not to call the user's
// interface function.

namespace PlatformSpecific
{
    OsAbstraction::OsAbstraction(SemaphoreHandle_t& processingMutex) :
        _processingMutex(processingMutex)
    {
        xTaskCreate(
            TimerHandlerTask,
            "TimerHandlerTask",
            4096,
            this,
            configMAX_PRIORITIES / 2,
            &_timerHandlerTask
        );
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
        // Get a new interface timer handle
        PlatformSpecific::IOsAbstraction::Input::TimerHandle interfaceTimerHandle = _currentInterfaceTimerHandle++;
        // Make a lookup from interface timer handle to user's timer ID, output interface and freeRTOS timer handle
        _timerInfoByInterfaceTimerHandle[interfaceTimerHandle] = std::make_tuple(timerId, iOutput, freeRtosTimerHandle);
        // Make a lookup from freeRTOS timer handle to interface timer handle
        _interfaceTimerHandleByFreeRtosTimerHandle[freeRtosTimerHandle] = interfaceTimerHandle;
        // Start the freeRTOS timer
        xTimerStart(freeRtosTimerHandle, 0);
        return interfaceTimerHandle;
    }

    void OsAbstraction::IpsOsAbstractionStopTimer(PlatformSpecific::IOsAbstraction::Input::TimerHandle timerHandle)
    {
        // Get the timer info (this should always succeed)
        auto timerInfoByInterfaceTimerHandleIterator = _timerInfoByInterfaceTimerHandle.find(timerHandle);
        if (timerInfoByInterfaceTimerHandleIterator != _timerInfoByInterfaceTimerHandle.end())
        {
            // Check if freeRTOS timer is still active
            // FreeRTOS may already have called the timer callback in which case it has been erased from the map
            auto const& timerInfo = timerInfoByInterfaceTimerHandleIterator->second;
            TimerHandle_t freeRtosTimerHandle = std::get<2>(timerInfo);
            if (_interfaceTimerHandleByFreeRtosTimerHandle.find(freeRtosTimerHandle) != _interfaceTimerHandleByFreeRtosTimerHandle.end())
            {
                // Although we call xTimerDelete the callback can still be called
                // Because we erase the handle from the map the callback will not act
                xTimerDelete(freeRtosTimerHandle, 0);
                // Erase lookup from freeRTOS timer handle to interface timer handle
                _interfaceTimerHandleByFreeRtosTimerHandle.erase(freeRtosTimerHandle);
            }
            _timerInfoByInterfaceTimerHandle.erase(timerHandle);
        }
    }

    void OsAbstraction::IpsOsAbstractionTrace(const char* tag, const char* text, va_list arguments)
    {
        vsnprintf(_traceBuffer, sizeof(_traceBuffer), text, arguments);
        ESP_LOGI(tag, "%s", _traceBuffer);
    }

    void OsAbstraction::TimerCallback(TimerHandle_t freeRtosTimerHandle)
    {
        // The pointer to the OsAbstraction instance is stored in the freeRTOS timer's ID
        // We will use the timer handle to identify the timer
        // The handle is unique from the moment the timer is created until the moment freeRTOS releases it
        // In case StopTimer already called xTimerDelete it is still possible that the callback is called,
        // however because freeRTOS has not yet released the handle we can safely use it to check the
        // _interfaceTimerHandleByFreeRtosTimerHandle map (and see it is not present there anymore)
        OsAbstraction* osAbstraction = (OsAbstraction*)pvTimerGetTimerID(freeRtosTimerHandle);
        xSemaphoreTake(osAbstraction->_processingMutex, portMAX_DELAY);
        auto interfaceTimerHandleByFreeRtosTimerHandleIterator = osAbstraction->_interfaceTimerHandleByFreeRtosTimerHandle.find(freeRtosTimerHandle);
        if (interfaceTimerHandleByFreeRtosTimerHandleIterator != osAbstraction->_interfaceTimerHandleByFreeRtosTimerHandle.end())
        {
            // Timer is still active
            // Delete it in freeRTOS
            xTimerDelete(freeRtosTimerHandle, 0);
            // Erase lookup from freeRTOS timer handle to interface timer handle
            osAbstraction->_interfaceTimerHandleByFreeRtosTimerHandle.erase(freeRtosTimerHandle);
            // Pass the interface timer handle to the TimerHandlerTask
            PlatformSpecific::IOsAbstraction::Input::TimerHandle interfaceTimerHandle = interfaceTimerHandleByFreeRtosTimerHandleIterator->second;
            osAbstraction->_timerHandlerTaskExpiredTimersList.push_back(interfaceTimerHandle);
            xTaskNotifyGive(osAbstraction->_timerHandlerTask);
        }
        xSemaphoreGive(osAbstraction->_processingMutex);
    }

    void OsAbstraction::TimerHandlerTask(void* pvParameters)
    {
        ((OsAbstraction*)pvParameters)->TimerHandlerTask();
    }

    void OsAbstraction::TimerHandlerTask()
    {
        while (true)
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            xSemaphoreTake(_processingMutex, portMAX_DELAY);
            for (auto const& interfaceTimerHandle : _timerHandlerTaskExpiredTimersList)
            {
                auto timerInfoByInterfaceTimerHandleIterator = _timerInfoByInterfaceTimerHandle.find(interfaceTimerHandle);
                if (timerInfoByInterfaceTimerHandleIterator != _timerInfoByInterfaceTimerHandle.end())
                {
                    // Timer is still active
                    // Call the handling function
                    auto const& timerInfo = timerInfoByInterfaceTimerHandleIterator->second;
                    PlatformSpecific::IOsAbstraction::Output* iOutput = std::get<1>(timerInfo);
                    int timerId = std::get<0>(timerInfo);
                    iOutput->IpsOsAbstractionHandleExpiredTimer(timerId);
                    // Erase the timer info
                    _timerInfoByInterfaceTimerHandle.erase(interfaceTimerHandle);
                }
            }
            _timerHandlerTaskExpiredTimersList.clear();
            xSemaphoreGive(_processingMutex);
        }
    }
}