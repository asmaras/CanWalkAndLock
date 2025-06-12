#pragma once

#include "PlatformIndependent/Commons/IStore.h"
#include "PlatformIndependent/Commons/Os.h"
#include "PlatformIndependent/Commons/IOs.h"
#include "PlatformIndependent/Commons/ICan.h"
#include "PlatformIndependent/Commons/IRemoteControl.h"
#include "PlatformIndependent/Commons/ISound.h"
#include <stdint.h>

namespace PlatformIndependent
{
    class CanWalProcessing :
        public PlatformIndependent::Commons::Os,
        public PlatformIndependent::Commons::ICan::Input,
        public PlatformIndependent::Commons::IRemoteControl::Input
    {
    public:
        CanWalProcessing(bool useRemoteControl);
        void SetOutputInterfaces(
            PlatformIndependent::Commons::IStore::Output* iStore,
            PlatformIndependent::Commons::IOs::Output* iOs,
            PlatformIndependent::Commons::ICan::Output* iCan,
            PlatformIndependent::Commons::IRemoteControl::Output* iRemoteControl,
            PlatformIndependent::Commons::ISound::Output* iSound
        );
        void Start();

        uint16_t _mirrorFoldFails = 0;
        uint16_t _mirrorFoldRecoveries = 0;

    private:
        // Function calls to input interfaces should always be mutually excluded
        // PlatformIndependent::Commons::Os
        void HandleExpiredTimer(int timerId, const char* timerName) override;
        // PlatformIndependent::Commons::ICan::Input
        void IPiCanHandleCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data) override;
        // PlatformIndependent::Commons::IRemoteControl::Input
        void IPiRemoteControlOperationFinished() override;

        // Storage for received statuses
        struct DoorOpenStatuses
        {
            bool frontDriverSideDoorIsOpen;
            bool rearDriverSideDoorIsOpen;
            bool frontPassengerSideDoorIsOpen;
            bool rearPassengerSideDoorIsOpen;
            bool bootIsOpen;
            bool bonnetIsOpen;
        };
        struct DoorLockStatuses
        {
            bool frontDriverSideDoorIsLocked;
            bool rearDriverSideDoorIsLocked;
            bool frontPassengerSideDoorIsLocked;
            bool rearPassengerSideDoorIsLocked;
        };
        struct
        {
            bool frontPassengerSeatIsOccupied;
            DoorOpenStatuses doorOpenStatuses;
            DoorLockStatuses doorLockStatuses;
            int vehicleSpeed;
            bool keyIsOutside;
            bool mirrorsAreFolded;
            unsigned char doorLockControlLast4Bytes[4];
        } _storedReportedStatuses;
        // State machine info
        // The states for the various state machines
        enum class FrontPassengerSeatState
        {
            vacated,
            occupied,
            occupiedStaleWaitUntilFrontPassengerDoorIsOpened
        } _frontPassengerSeatState;
        const char* ToString(FrontPassengerSeatState frontPassengerSeatState);
        enum class WalCancelState
        {
            lockedOrNoDoorOpenedAfterUnlock,
            cancellationByRemoteControlPossible,
            walCancelled
        } _walCancelState;
        const char* ToString(WalCancelState walCancelState);
        enum class DoorOpenSequenceAfterUnlockState
        {
            lockedOrNoDoorOpenedAfterUnlock,
            driverDoorOpenedAfterUnlock,
            onlyNonDriverDoorOpenedAfterUnlock
        } _doorOpenSequenceAfterUnlockState;
        const char* ToString(DoorOpenSequenceAfterUnlockState doorOpenSequenceAfterUnlockState);
        enum class MainWalState
        {
            noGo,
            go,
            executing
        } _mainWalState;
        const char* ToString(MainWalState mainWalState);
        // Event types
        enum class EventType
        {
            timerExpiry,
            frontPassengerSideSeatOccupationStatus,
            doorOpenStatus,
            doorLockStatus,
            remoteControlButtonPressed,
            doorHandleButtonPressed,
            vehicleSpeed,
            keyLocation,
            windowRoofAndMirrorControl,
            mirrorFoldToggleRepeat
        };
        // Timers
        enum class TimerPeriod
        {
            frontPassengerSideSeatOccupationStatusStale = 10000,
            lockCarWaitShort = 5000,
            lockCarWaitLong = 30000,
            intermediateCountdownSoundInterval = 2000,
            foldMirrorsCanMessageWait = 1000,
            checkMirrorsFolded = 1000,
            closeWindowsAndRoofCanMessageWaitStandardInterval = 100,
            closeWindowsAndRoofCanMessageWaitAfterStop = 500,
            stopWalExecutionAfterClosingWindowsAndRoof = 25000,
            stopWalExecutionAfterLocking = 5000,
            mirrorFoldRepeatedToggleMaximumInterval = 1000,
            cancelForgetPeriod = 30000
        };
        // Timer variables
        struct Timers
        {
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(frontPassengerSideSeatOccupationStatusStale);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(lockCarWait);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(intermediateCountdownSoundInterval);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(foldMirrorsCanMessageWait);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(checkMirrorsFolded);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(closeWindowsAndRoofCanMessageWait);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(stopWalExecution);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(mirrorFoldRepeatedToggleMaximumInterval);
            PLATFORMINDEPENDENCECOMMONS_OSTIMER(cancelForgetPeriod);
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
            struct
            {
                int year;
                int month;
                int day;
                int hour;
                int minute;
                int second;
            } dateTime;
        };
        void HandleEvent(Event event);
        void TraceEvent(Event event);
        bool DoorsBootAndBonnetAreClosed();
        bool AllDoorsAreLocked();
        bool EventForGoCondition(EventType eventType, bool frontPassengerSeatStateChanged, bool walCancelStateChanged);
        bool GoConditionActive();
        void LockDoors();
        void SendLockDoorsMessage();
        void SendFoldMirrorsMessage();
        void SendCloseWindowsAndRoofMessage();
        void SendStopCloseWindowsAndRoofAndFoldMirrorsMessage();
        // Nicer version with parameter type TimerPeriod (base class takes an int)
        void StartTimer(int timerId, TimerPeriod timerPeriod);

        const bool _useRemoteControl;
        PlatformIndependent::Commons::IStore::Output* _iStore;
        PlatformIndependent::Commons::ICan::Output* _iCan;
        PlatformIndependent::Commons::IRemoteControl::Output* _iRemoteControl;
        PlatformIndependent::Commons::ISound::Output* _iSound;
        bool _storeValueEnableWal;
        bool _storeValueMayCloseWindowsAndRoof;
        bool _performingRemoteControlOperation;
        static constexpr int _drivingSpeedThreshold = 5 * 10;
        bool _mirrorFoldFailed;
    };
}