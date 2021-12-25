#include "OsAbstraction.h"
#include <esp_log.h>

OsAbstraction::OsAbstraction()
{

}

void OsAbstraction::SetOutputInterface(IOsAbstraction::Output* iOutput)
{
    _iOutput = iOutput;
}

void OsAbstraction::StartTimer(int timerId, int timerPeriodInMilliSeconds)
{
    StopTimer(timerId);

    TimerHandle_t timerHandle = xTimerCreate(
        "OsAbstraction",
        pdMS_TO_TICKS(timerPeriodInMilliSeconds),
        pdFALSE,
        this,
        CommonHandling::TimerCallback
    );
    _timerHandleById[timerId] = timerHandle;
    _commonHandling._timerIdByHandle[timerHandle] = timerId;
    xTimerStart(timerHandle, 0);
}

void OsAbstraction::StopTimer(int timerId)
{
    auto timerHandleByIdIterator = _timerHandleById.find(timerId);
    if (timerHandleByIdIterator != _timerHandleById.end())
    {
        // Be sure to store the timerId because the iterator will be destroyed by the erase function
        TimerHandle_t timerHandle = timerHandleByIdIterator->second;
        // Although we call xTimerDelete the callback can still be called
        // Because we erase the handle from the map the callback will not act
        xTimerDelete(timerHandleByIdIterator->second, 0);
        _timerHandleById.erase(timerId);
        _commonHandling._timerIdByHandle.erase(timerHandle);
    }
}

void OsAbstraction::Trace(const char* tag, const char* text, va_list arguments)
{
    _commonHandling.Trace(tag, text, arguments);
}

OsAbstraction::CommonHandling::CommonHandling(SemaphoreHandle_t& processingMutex) :
    _processingMutex(processingMutex)
{
    
}

void OsAbstraction::CommonHandling::Trace(const char* tag, const char* text, va_list arguments)
{
    vsnprintf(_traceBuffer, sizeof(_traceBuffer), text, arguments);
    ESP_LOGI(tag, "%s", _traceBuffer);
}

void OsAbstraction::CommonHandling::TimerCallback(TimerHandle_t timerHandle)
{
    // The pointer to the OsAbstraction instance is stored in the freeRTOS timer's ID
    // We will use the timer handle to identify the timer
    // The handle is unique from the moment the timer is created until the moment freeRTOS releases it
    // In case StopTimer already called xTimerDelete it is still possible that the callback is called,
    // however because freeRTOS has not yet released the handle we can safely use it to check the
    // _timerIdByHandle map (and see it is not present there anymore)
    OsAbstraction* osAbstraction = (OsAbstraction*)pvTimerGetTimerID(timerHandle);
    xSemaphoreTake(_commonHandling._processingMutex, portMAX_DELAY);
    auto timerIdByHandleIterator = _commonHandling._timerIdByHandle.find(timerHandle);
    if (timerIdByHandleIterator != _commonHandling._timerIdByHandle.end())
    {
        // Be sure to store the timerId because the iterator will be destroyed by the erase function
        int timerId = timerIdByHandleIterator->second;
        // Cleanup first before calling the function to handle the timer expiry
        // If the handling function starts the timer again we will not inadvertedly clean it up
        xTimerDelete(timerHandle, 0);
        osAbstraction->_timerHandleById.erase(timerId);
        _commonHandling._timerIdByHandle.erase(timerHandle);
        // Finally call the handling function
        osAbstraction->_iOutput->HandleExpiredTimer(timerId);
    }
    xSemaphoreGive(_commonHandling._processingMutex);
}
