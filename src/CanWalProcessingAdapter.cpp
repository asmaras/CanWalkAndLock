#include "CanWalProcessingAdapter.h"

CanWalProcessingAdapter::CanWalProcessingAdapter()
{
    _canWalProcessingMutex = xSemaphoreCreateMutex();
}

void CanWalProcessingAdapter::HandleCanMessage(int id, int length, const unsigned char* data)
{
    _iCanWalProcessing->HandleCanMessage(id, length, data);
}

void CanWalProcessingAdapter::StartTimer(int timerId, int timerPeriodInMilliSeconds)
{
    StopTimer(timerId);
    
    TimerHandle_t timerHandle = xTimerCreate(
        "CanWalProcessingAdapter",
        pdMS_TO_TICKS(timerPeriodInMilliSeconds),
        pdFALSE,
        this,
        TimerCallback
    );
    _timerHandleById[timerId] = timerHandle;
    _timerIdByHandle[timerHandle] = timerId;
    xTimerStart(timerHandle, 0);
}

void CanWalProcessingAdapter::StopTimer(int timerId)
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
        _timerIdByHandle.erase(timerHandle);
    }
}

void CanWalProcessingAdapter::SendCanMessage(int id, int length, const unsigned char* data)
{
    _iCanInterface->SendMessage(id, length, data);
}

void CanWalProcessingAdapter::PlaySound(Sound sound)
{
    
}

void CanWalProcessingAdapter::StopPlayingSound()
{
    
}

void CanWalProcessingAdapter::Trace(const char* formattedText, ...)
{
    va_list arguments;
    va_start(arguments, formattedText);
    vprintf(formattedText, arguments);
    printf("\n");
    va_end(arguments);
}

void CanWalProcessingAdapter::TimerCallback(TimerHandle_t timerHandle)
{
    // The pointer to the CanWalProcessingAdapter instance is stored in the freeRTOS timer's ID
    // We will use the timer handle to identify the timer
    // The handle is unique from the moment the timer is created until the moment freeRTOS releases it
    // In case StopTimer already called xTimerDelete it is still possible that the callback is called,
    // however because freeRTOS has not yet released the handle we can safely use it to check the
    // _timerIdByHandle map (and see it is not present there anymore)
    CanWalProcessingAdapter* canWalProcessingAdapter = (CanWalProcessingAdapter*)pvTimerGetTimerID(timerHandle);
    xSemaphoreTake(canWalProcessingAdapter->_canWalProcessingMutex, portMAX_DELAY);
    auto timerIdByHandleIterator = canWalProcessingAdapter->_timerIdByHandle.find(timerHandle);
    if (timerIdByHandleIterator != canWalProcessingAdapter->_timerIdByHandle.end())
    {
        // Be sure to store the timerId because the iterator will be destroyed by the erase function
        int timerId = timerIdByHandleIterator->second;
        // Cleanup first before calling the function to handle the timer expiry
        // If the handling function starts the timer again we will not inadvertedly clean it up
        xTimerDelete(timerHandle, 0);
        canWalProcessingAdapter->_timerHandleById.erase(timerId);
        canWalProcessingAdapter->_timerIdByHandle.erase(timerHandle);
        // Finally call the handling function
        canWalProcessingAdapter->_iCanWalProcessing->HandleExpiredTimer(timerId);
    }
    xSemaphoreGive(canWalProcessingAdapter->_canWalProcessingMutex);
}
