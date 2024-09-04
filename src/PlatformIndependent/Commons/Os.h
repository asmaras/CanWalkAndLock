#pragma once

#include "IOs.h"

namespace PlatformIndependent::Commons
{
    class Os : public PlatformIndependent::Commons::IOs::Input
    {
    protected:
        Os();
        void SetOutputInterface(PlatformIndependent::Commons::IOs::Output* iOs);
        void StartTimer(int timerId, int timerPeriod);
        void StopTimer(int timerId);
        bool TimerRunning(int timerId);
        void Trace(const char* text, ...);

        // PlatformIndependent::Commons::IOs::Input
        void IPiOsHandleExpiredTimer(int timerId) override;
        
        virtual void HandleExpiredTimer(int timerId, const char* timerName) = 0;

        // Base type holding timer data
        class Timer
        {
        public:
            Timer(int timerId, const char* name);
            static Timer* FindTimer(int timerId);

            const int _timerId;
            const char* const _name;
            bool _running;

        private:
            static Timer* _lastTimer;
            Timer* const _previousTimer;
        };

        // Macro for creating a timer variable
#define PLATFORMINDEPENDENCECOMMONS_OSTIMER(name) \
        static const int name = __LINE__; \
        class _TimerClass_##name : public Timer \
        { \
        public: \
            _TimerClass_##name() : \
                Timer(name, #name) \
            {} \
        } _timerVariable_##name;

    private:
        PlatformIndependent::Commons::IOs::Output* _iOs;
    };
}