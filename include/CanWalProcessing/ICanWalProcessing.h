#pragma once

class ICanWalProcessing
{
public:
    class Os
    {
    public:
        class Input
        {
        public:
            // Function calls to input interfaces should always be mutually excluded
            virtual void HandleExpiredTimer(int timerId) = 0;
        };
        
        class Output
        {
        public:
            virtual void StartTimer(int timerId, int timerPeriodInMilliSeconds) = 0;
            virtual void StopTimer(int timerId) = 0;
            virtual void Trace(const char* text, ...) = 0;
        };
    };

    class Can
    {
    public:
        class Input
        {
        public:
            // Function calls to input interfaces should always be mutually excluded
            virtual void HandleCanMessage(int id, int length, const unsigned char* data) = 0;
        };
        
        class Output
        {
        public:
            virtual void SendCanMessage(int id, int length, const unsigned char* data) = 0;
        };
    };

    class Sound
    {
    public:
        class Output
        {
        public:
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
        };
    };
};
