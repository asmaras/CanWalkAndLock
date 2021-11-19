#pragma once

class ICanWalProcessing
{
public:
    // Function calls should always be mutually excluded
    virtual void HandleCanMessage(int id, int length, const unsigned char* data) = 0;
    virtual void HandleExpiredTimer(int timerId) = 0;
};

class ICanWalProcessingOut
{
public:
    virtual void StartTimer(int timerId, int timerPeriodInMilliSeconds) = 0;
    virtual void StopTimer(int timerId) = 0;
    virtual void SendCanMessage(int id, int length, const unsigned char* data) = 0;
    enum class Sound
    {
        goFast,
        goSlow,
        goFastWindowsNotClosed,
        goSlowWindowsNotCLosed,
        intermediateCountdown
    };
    // This function should play a sound immediately and interrupt a sound being played, if any
    virtual void PlaySound(Sound sound) = 0;
    virtual void StopPlayingSound() = 0;
    virtual void Trace(const char* text, ...) = 0;
};
