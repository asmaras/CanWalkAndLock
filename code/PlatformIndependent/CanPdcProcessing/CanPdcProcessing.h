#pragma once

#include "PlatformIndependent/Commons/Os.h"
#include "PlatformIndependent/Commons/IOs.h"
#include "PlatformIndependent/Commons/ICan.h"
#include "PlatformIndependent/Commons/IPdc.h"

namespace PlatformIndependent
{
    class CanPdcProcessing :
        public PlatformIndependent::Commons::Os,
        public PlatformIndependent::Commons::ICan::Input
    {
    public:
        CanPdcProcessing();
        void SetOutputInterfaces(
            PlatformIndependent::Commons::IOs::Output* iOs,
            PlatformIndependent::Commons::ICan::Output* iCan,
            PlatformIndependent::Commons::IPdc::Output* iPdc
        );

    private:
        // Function calls to input interfaces should always be mutually excluded
        // PlatformIndependent::Commons::Os
        void HandleExpiredTimer(int timerId, const char* timerName) override;
        // PlatformIndependent::Commons::ICan::Input
        void IPiCanHandleCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data) override;

        // Storage for received statuses
        struct
        {
            int vehicleSpeed;
            char automaticGearShifterPosition;
        } _storedReportedStatuses;
        // State machine info
        enum class BuzzerState
        {
            enabled,
            disabled
        } _buzzerState;
        const char* ToString(BuzzerState buzzerState);
        // Event types
        enum class EventType
        {
            timerExpiry,
            vehicleSpeed,
            automaticGearShifterPosition
        };
        // Timers
        enum class TimerPeriod
        {
            buzzerOffWait = 2000
        };
        // Timer variables
        struct Timers
        {
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(buzzerOffWait);
        } _timers;
        // Event struct holding event type and parameters
        // Stored statuses are not included as parameters to make code less complex because it only has
        // to look at the stored statuses
        struct Event
        {
            EventType type;
            struct
            {
                int timerId;
                const char* timerName;
            } timerExpiry;
        };
        void HandleEvent(Event event);
        void TraceEvent(Event event);
        bool CheckShifterChangedToDrivingPosition(char position);
        bool CheckShifterChangedToParkingPosition(char position);
        // Nicer version with parameter type TimerPeriod (base class takes an int)
        void StartTimer(int timerId, TimerPeriod timerPeriod);

        PlatformIndependent::Commons::IPdc::Output* _iPdc;
        static constexpr int _switchOffSpeed = 20 * 10;
    };
}