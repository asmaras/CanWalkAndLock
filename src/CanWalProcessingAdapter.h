#pragma once

#include "ICanWalProcessingAdapter.h"
#include "ICanWalProcessing.h"
#include "ICanInterface.h"
#include "ISoundPlayer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include <map>

class CanWalProcessingAdapter : public ICanWalProcessingOut, public ICanWalProcessingAdapter
{
public:
    CanWalProcessingAdapter();

    ICanWalProcessing* _iCanWalProcessing = nullptr;
    ICanInterface* _iCanInterface = nullptr;
    ISoundPlayer* _iSoundPlayer = nullptr;

    void HandleCanMessage(int id, int length, const unsigned char* data) override;

private:
    void StartTimer(int timerId, int timerPeriodInMilliSeconds) override;
    void StopTimer(int timerId) override;
    void SendCanMessage(int id, int length, const unsigned char* data) override;
    void PlaySound(Sound sound) override;
    void StopPlayingSound() override;
    void Trace(const char* text, ...) override;

    SemaphoreHandle_t _canWalProcessingMutex;
    std::map<int, TimerHandle_t> _timerHandleById;
    std::map<TimerHandle_t, int> _timerIdByHandle;
    static void TimerCallback(TimerHandle_t timerHandle);
};