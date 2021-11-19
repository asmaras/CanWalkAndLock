#pragma once

#include "ICanWalProcessing.h"

class CanWalProcessing : public ICanWalProcessing
{
public:
    CanWalProcessing();

    ICanWalProcessingOut* _iCanWalProcessingOut = nullptr;

    // Function calls should always be mutually excluded
    void HandleCanMessage(int id, int length, const unsigned char* data) override;
    void HandleExpiredTimer(int timerId) override;

private:
    // CAN IDs
    enum class CanId
    {
        frontPassengerSideDoorStatus = 0x0E2,
        rearPassengerSideDoorStatus = 0x0E6,
        frontDriverSideDoorStatus = 0x0EA,
        rearDriverSideDoorStatus = 0x0EE,
        mirrorFoldStatus = 0x0F6,
        ignitionAndKeyStatus = 0x130,
        vehicleSpeed = 0x1B4,
        remoteControlInput = 0x23A,
        windowRoofAndMirrorControl = 0x26E,
        doorLockControl = 0x2A0,
        dateTime = 0x2F8,
        seatbeltAndSeatOccupancyStatus = 0x2FA,
        doorStatus = 0x2FC
    };
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
        noDoorOpenedAfterUnlockCancelNotPossible,
        doorOpenedAfterUnlockCancelPossible,
        walCancelled
    } _walCancelState;
    const char* ToString(WalCancelState walCancelState);
    enum class DoorOpenSequenceAfterUnlockState
    {
        noDoorOpenedAfterUnlock,
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
        frontPassengerSeatOccupationStatus,
        doorOpenStatus,
        doorLockStatus,
        remoteControlUnlockButtonPressed,
        remoteControlLockButtonPressed,
        vehicleSpeed,
        keyLocation,
        dateTime
    };
    // Timers
    enum class TimerPeriod
    {
        frontPassengerSeatOccupationStatusStale = 10000,
        lockCarWaitShort = 5000,
        lockCarWaitLong = 30000,
        intermediateCountdownSoundInterval = 2000,
        closeWindowsAndRoofCanMessageInterval = 100,
        stopWalExecutionShort = 1000,
        stopWalExecutionLong = 14000
    };
    // Base type holding timer data
    class Timer
    {
    public:
        Timer(int timerId, const char* name);

        static Timer* FindTimer(int timerId);

        const int _timerId;
        const char* _name;
        bool _running;
        static Timer* _lastTimer;
        Timer* const _previousTimer;
    };
    // Macro for creating a timer variable
#define CANWALPROCESSINGTIMER(name) \
    static const int name = __LINE__; \
    class _TimerClass_##name : public Timer \
    { \
    public: \
        _TimerClass_##name() : \
            Timer(name, #name) \
        {} \
    } _timerVariable_##name;
    // The actual timer variables
    struct Timers
    {
        CANWALPROCESSINGTIMER(frontPassengerSeatOccupationStatusStale);
        CANWALPROCESSINGTIMER(lockCarWait);
        CANWALPROCESSINGTIMER(intermediateCountdownSoundInterval);
        CANWALPROCESSINGTIMER(closeWindowsAndRoofCanMessageInterval);
        CANWALPROCESSINGTIMER(stopWalExecution);
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
    void StartTimer(int timerId, TimerPeriod timerPeriod);
    void StopTimer(int timerId);
    bool TimerRunning(int timerId);
    bool DoorsBootAndBonnetAreClosed();
    bool AllDoorsAreLocked();
    bool EventForGoCondition(EventType eventType, bool frontPassengerSeatStateChanged, bool walCancelStateChanged);
    bool GoConditionActive();
    void SendLockDoorsMessage();
    void SendCloseWindowsAndRoofAndFoldMirrorsMessage();
};
