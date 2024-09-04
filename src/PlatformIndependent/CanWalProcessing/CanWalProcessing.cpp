#include "CanWalProcessing.h"

namespace PlatformIndependent
{
    CanWalProcessing::CanWalProcessing(bool useRemoteControl) :
        _useRemoteControl(useRemoteControl)
    {
        // At power-up we assume all doors to be closed and locked,
        // the key to be outside, the mirrors to be unfolded and the handbrake to be active
        // This should be consistent in the stored reported statuses as well as in the states
        _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.bootIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen = false;
        _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked = true;
        _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked = true;
        _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked = true;
        _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked = true;
        _storedReportedStatuses.keyIsOutside = true;
        _storedReportedStatuses.mirrorsAreFolded = false;
        for (int index = 0; index < 4; index++)
        {
            _storedReportedStatuses.doorLockControlLast4Bytes[index] = 0;
        }
        _storedReportedStatuses.handbrakeIsActive = true;
        _frontPassengerSeatState = FrontPassengerSeatState::vacated;
        _walCancelState = WalCancelState::lockedOrNoDoorOpenedAfterUnlock;
        _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::lockedOrNoDoorOpenedAfterUnlock;
        _mainWalState = MainWalState::noGo;
        
        _iCan = nullptr;
        _iRemoteControl = nullptr;
        _iSound = nullptr;
        _storeValueEnableWal = false;
        _storeValueMayCloseWindowsAndRoof = false;
        _performingRemoteControlOperation = false;
    }

    void CanWalProcessing::SetOutputInterfaces(
        PlatformIndependent::Commons::IStore::Output* iStore,
        PlatformIndependent::Commons::IOs::Output* iOs,
        PlatformIndependent::Commons::ICan::Output* iCan,
        PlatformIndependent::Commons::IRemoteControl::Output* iRemoteControl,
        PlatformIndependent::Commons::ISound::Output* iSound
    )
    {
        _iStore = iStore;
        PlatformIndependent::Commons::Os::SetOutputInterface(iOs);
        _iCan = iCan;
        _iRemoteControl = iRemoteControl;
        _iSound = iSound;
    }

    void CanWalProcessing::Start()
    {
        _storeValueEnableWal = _iStore->IPiStoreGetEnableWAL();
        Trace("Getting _storeValueEnableWal(%s)", _storeValueEnableWal ? "true" : "false");
        _storeValueMayCloseWindowsAndRoof = _iStore->IPiStoreGetMayCloseWindowsAndRoof();
        Trace("Getting _storeValueMayCloseWindowsAndRoof(%s)", _storeValueMayCloseWindowsAndRoof ? "true" : "false");
        // Timer for test code
        // StartTimer(Timers::intermediateCountdownSoundInterval, TimerPeriod::intermediateCountdownSoundInterval);
    }

    void CanWalProcessing::HandleExpiredTimer(int timerId, const char* timerName)
    {
        Event event;
        event.type = EventType::timerExpiry;
        event.timerExpiry.timerId = timerId;
        event.timerExpiry.timerName = timerName;
        HandleEvent(event);
    }

    void CanWalProcessing::IPiCanHandleCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data)
    {
        switch (id)
        {
        case PlatformIndependent::Commons::ICan::CanId::frontPassengerSideDoorStatus:
        {
            // The open status is in bit 0 of byte 3
            bool doorIsOpen = (data[3] & 0x01) != 0;
            if (doorIsOpen != _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen = doorIsOpen;
                Event event;
                event.type = EventType::doorOpenStatus;
                HandleEvent(event);
            }
            // The lock status is in bits 0 and 1 of byte 0
            bool doorIsLocked = (data[0] & 0x03) != 1;
            if (doorIsLocked != _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked)
            {
                _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked = doorIsLocked;
                Event event;
                event.type = EventType::doorLockStatus;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::rearPassengerSideDoorStatus:
        {
            // The open status is in bit 0 of byte 3
            bool doorIsOpen = (data[3] & 0x01) != 0;
            if (doorIsOpen != _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen = doorIsOpen;
                Event event;
                event.type = EventType::doorOpenStatus;
                HandleEvent(event);
            }
            // The lock status is in bits 0 and 1 of byte 0
            bool doorIsLocked = (data[0] & 0x03) != 1;
            if (doorIsLocked != _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked)
            {
                _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked = doorIsLocked;
                Event event;
                event.type = EventType::doorLockStatus;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::frontDriverSideDoorStatus:
        {
            // The open status is in bit 0 of byte 3
            bool doorIsOpen = (data[3] & 0x01) != 0;
            if (doorIsOpen != _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen = doorIsOpen;
                Event event;
                event.type = EventType::doorOpenStatus;
                HandleEvent(event);
            }
            // The lock status is in bits 0 and 1 of byte 0
            bool doorIsLocked = (data[0] & 0x03) != 1;
            if (doorIsLocked != _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked)
            {
                _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked = doorIsLocked;
                Event event;
                event.type = EventType::doorLockStatus;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::rearDriverSideDoorStatus:
        {
            // The open status is in bit 0 of byte 3
            bool doorIsOpen = (data[3] & 0x01) != 0;
            if (doorIsOpen != _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen = doorIsOpen;
                Event event;
                event.type = EventType::doorOpenStatus;
                HandleEvent(event);
            }
            // The lock status is in bits 0 and 1 of byte 0
            bool doorIsLocked = (data[0] & 0x03) != 1;
            if (doorIsLocked != _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked)
            {
                _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked = doorIsLocked;
                Event event;
                event.type = EventType::doorLockStatus;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::mirrorFoldStatus:
        {
            // Byte 0 will be F7 when the mirrors are being folded
            bool mirrorsAreFolded = data[0] == 0xF7;
            if (mirrorsAreFolded != _storedReportedStatuses.mirrorsAreFolded)
            {
                _storedReportedStatuses.mirrorsAreFolded = mirrorsAreFolded;
                // This only needs to be stored, no event handling needed
                Trace("Status mirrorFoldStatus(%d)", mirrorsAreFolded);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::ignitionAndKeyStatus:
        {
            // When the key is outside bytes 1 and 3 will be 0E/04 or 01/06 (door handle button is pushed)
            bool keyIsOutside =
                (data[1] == 0x0E && data[3] == 0x04) ||
                (data[1] == 0x01 && data[3] == 0x06);
            if (keyIsOutside != _storedReportedStatuses.keyIsOutside)
            {
                _storedReportedStatuses.keyIsOutside = keyIsOutside;
                Event event;
                event.type = EventType::keyLocation;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::vehicleSpeed:
        {
            // The vehicle speed is in the first 12 bits of bytes 0 and 1
            // The unit is 0.1 km/h
            int vehicleSpeed = (data[0] + ((data[1] & 0x0F) << 8));
            if (vehicleSpeed != _storedReportedStatuses.vehicleSpeed)
            {
                _storedReportedStatuses.vehicleSpeed = vehicleSpeed;
                Event event;
                event.type = EventType::vehicleSpeed;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::remoteControlAndDoorHandleInput:
        {
            // Bit 0 of byte 2 represents the unlock button
            // Bit 2 of byte 2 represents the lock button
            if ((data[2] & 0x01) != 0 || (data[2] & 0x04) != 0)
            {
                Event event;
                // Bits 0 and 1 of byte 1 are zero when the input comes from the remote control
                // They are 3 when it comes from the door handle
                if ((data[1] & 0x03) == 0)
                {
                    if (_performingRemoteControlOperation)
                    {
                        Trace("Not handling remoteControlAndDoorHandleInput because a remote control operation is being performed");
                    }
                    else
                    {
                        event.type = EventType::remoteControlButtonPressed;
                    }
                }
                else
                {
                    event.type = EventType::doorHandleButtonPressed;
                }
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::windowRoofAndMirrorControl:
        {
            Event event;
            event.type = EventType::windowRoofAndMirrorControl;
            HandleEvent(event);
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::doorLockControl:
        {
            if (length == 8)
            {
                for (int index = 0; index < 4; index++)
                {
                    _storedReportedStatuses.doorLockControlLast4Bytes[index] = data[index + 4];
                    // This only needs to be stored, no event handling needed
                }
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::dateTime:
        {
            // No event handling needed
            Trace(
                "DateTime(%d-%02d-%02d %02d:%02d:%02d)",
                data[5] + (data[6] << 8),
                data[4] >> 4,
                data[3],
                data[0],
                data[1],
                data[2]
                );
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::seatbeltAndSeatOccupancyStatus:
        {
            // Byte 1 is 08 when the seat is vacated
            bool seatIsOccupied = data[1] != 0x08;
            if (seatIsOccupied != _storedReportedStatuses.frontPassengerSeatIsOccupied)
            {
                _storedReportedStatuses.frontPassengerSeatIsOccupied = seatIsOccupied;
                Event event;
                event.type = EventType::frontPassengerSideSeatOccupationStatus;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::doorStatus:
        {
            Event event;
            bool handleDoorOpenStatusEvent = false;
            // The front driver side door open status is in bit 0 of byte 1
            bool frontDriverSideDoorIsOpen = (data[1] & 0x01) != 0;
            if (frontDriverSideDoorIsOpen != _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen = frontDriverSideDoorIsOpen;
                handleDoorOpenStatusEvent = true;
            }
            // The front passenger side door open status is in bit 2 of byte 1
            bool frontPassengerSideDoorIsOpen = (data[1] & 0x04) != 0;
            if (frontPassengerSideDoorIsOpen != _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen = frontPassengerSideDoorIsOpen;
                handleDoorOpenStatusEvent = true;
            }
            // The rear driver side door open status is in bit 4 of byte 1
            bool rearDriverSideDoorIsOpen = (data[1] & 0x10) != 0;
            if (rearDriverSideDoorIsOpen != _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen = rearDriverSideDoorIsOpen;
                handleDoorOpenStatusEvent = true;
            }
            // The rear passenger side door open status is in bit 6 of byte 1
            bool rearPassengerSideDoorIsOpen = (data[1] & 0x40) != 0;
            if (rearPassengerSideDoorIsOpen != _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen = rearPassengerSideDoorIsOpen;
                handleDoorOpenStatusEvent = true;
            }
            // The boot open status is in bit 0 of byte 2
            bool bootIsOpen = (data[2] & 0x01) != 0;
            if (bootIsOpen != _storedReportedStatuses.doorOpenStatuses.bootIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.bootIsOpen = bootIsOpen;
                handleDoorOpenStatusEvent = true;
            }
            // The bonnet open status is in bit 2 of byte 2
            bool bonnetIsOpen = (data[2] & 0x04) != 0;
            if (bonnetIsOpen != _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen)
            {
                _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen = bonnetIsOpen;
                handleDoorOpenStatusEvent = true;
            }
            if (handleDoorOpenStatusEvent)
            {
                event.type = EventType::doorOpenStatus;
                HandleEvent(event);
            }
        }
        break;
        case PlatformIndependent::Commons::ICan::CanId::handbrakeStatus:
        {
            static int handbrakeToggleRepeatCount = 0;
            // Bits 0 and 1 of byte 0 are 2 when the handbrake is active
            bool handbrakeIsActive = (data[0] & 0x03) == 2;
            if (handbrakeIsActive != _storedReportedStatuses.handbrakeIsActive)
            {
                _storedReportedStatuses.handbrakeIsActive = handbrakeIsActive;
                if (TimerRunning(Timers::handbrakeToggleRepeat))
                {
                    handbrakeToggleRepeatCount++;
                    Trace("handbrakeToggleRepeatCount(%d)", handbrakeToggleRepeatCount);
                    if (handbrakeToggleRepeatCount == 2)
                    {
                        Event event;
                        event.type = EventType::handbrakeToggleRepeat;
                        HandleEvent(event);
                    }
                    else if (handbrakeToggleRepeatCount == 6)
                    {
                        _storeValueEnableWal = !_storeValueEnableWal;
                        Trace("Setting _storeValueEnableWal(%s)", _storeValueEnableWal ? "true" : "false");
                        _iStore->IPiStoreSetEnableWAL(_storeValueEnableWal);
                    }
                }
                else
                {
                    handbrakeToggleRepeatCount = 1;
                }
                StartTimer(Timers::handbrakeToggleRepeat, TimerPeriod::handbrakeToggleRepeatTime);
            }
        }
        break;
        default:
            break;
        }
    }

    void CanWalProcessing::IPiRemoteControlOperationFinished()
    {
        _performingRemoteControlOperation = false;
    }

    const char* CanWalProcessing::ToString(FrontPassengerSeatState frontPassengerSeatState)
    {
        switch (frontPassengerSeatState)
        {
        case FrontPassengerSeatState::vacated: return "vacated";
        case FrontPassengerSeatState::occupied: return "occupied";
        case FrontPassengerSeatState::occupiedStaleWaitUntilFrontPassengerDoorIsOpened: return "occupiedStaleWaitUntilFrontPassengerDoorIsOpened";
        default: return "";
        }
    }

    const char* CanWalProcessing::ToString(WalCancelState walCancelState)
    {
        switch (walCancelState)
        {
        case WalCancelState::lockedOrNoDoorOpenedAfterUnlock: return "lockedOrNoDoorOpenedAfterUnlock";
        case WalCancelState::cancellationBeforeExecutionPossible: return "cancellationBeforeExecutionPossible";
        case WalCancelState::walCancelled: return "walCancelled";
        default: return "";
        }
    }

    const char* CanWalProcessing::ToString(DoorOpenSequenceAfterUnlockState doorOpenSequenceAfterUnlockState)
    {
        switch (doorOpenSequenceAfterUnlockState)
        {
        case DoorOpenSequenceAfterUnlockState::lockedOrNoDoorOpenedAfterUnlock: return "lockedOrNoDoorOpenedAfterUnlock";
        case DoorOpenSequenceAfterUnlockState::driverDoorOpenedAfterUnlock: return "driverDoorOpenedAfterUnlock";
        case DoorOpenSequenceAfterUnlockState::onlyNonDriverDoorOpenedAfterUnlock: return "onlyNonDriverDoorOpenedAfterUnlock";
        default: return "";
        }
    }

    const char* CanWalProcessing::ToString(MainWalState mainWalState)
    {
        switch (mainWalState)
        {
        case MainWalState::noGo: return "noGo";
        case MainWalState::go: return "go";
        case MainWalState::executing: return "executing";
        default: return "";
        }
    }

    void CanWalProcessing::HandleEvent(Event event)
    {
        TraceEvent(event);

        auto previousFrontPassengerSeatState = _frontPassengerSeatState;
        bool frontPassengerSeatStateChanged = false;
        switch (_frontPassengerSeatState)
        {
        case FrontPassengerSeatState::vacated:
            switch (event.type)
            {
            case EventType::frontPassengerSideSeatOccupationStatus:
                if (_storedReportedStatuses.frontPassengerSeatIsOccupied)
                {
                    StartTimer(Timers::frontPassengerSideSeatOccupationStatusStale, TimerPeriod::frontPassengerSideSeatOccupationStatusStale);
                    _frontPassengerSeatState = FrontPassengerSeatState::occupied;
                }
                break;
            default:
                break;
            }
            break;
        case FrontPassengerSeatState::occupied:
            switch (event.type)
            {
            case EventType::frontPassengerSideSeatOccupationStatus:
                if (_storedReportedStatuses.frontPassengerSeatIsOccupied)
                {
                    // Every time we receive a new occupied status the timer must be restarted
                    StartTimer(Timers::frontPassengerSideSeatOccupationStatusStale, TimerPeriod::frontPassengerSideSeatOccupationStatusStale);
                }
                else
                {
                    StopTimer(Timers::frontPassengerSideSeatOccupationStatusStale);
                    _frontPassengerSeatState = FrontPassengerSeatState::vacated;
                }
                break;
            case EventType::timerExpiry:
                // When the timer expires the occupied status becomes stale
                if (event.timerExpiry.timerId == Timers::frontPassengerSideSeatOccupationStatusStale)
                {
                    // We can assume the seat is vacated when the status is stale and the front passenger door is open
                    if (_storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen)
                    {
                        _frontPassengerSeatState = FrontPassengerSeatState::vacated;
                    }
                    else
                    {
                        // Wait until the door is opened
                        _frontPassengerSeatState = FrontPassengerSeatState::occupiedStaleWaitUntilFrontPassengerDoorIsOpened;
                    }
                }
                break;
            default:
                break;
            }
            break;
        case FrontPassengerSeatState::occupiedStaleWaitUntilFrontPassengerDoorIsOpened:
            switch (event.type)
            {
            case EventType::doorOpenStatus:
                // We can assume the seat is vacated when the status is stale and the front passenger door is open
                if (_storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen)
                {
                    _frontPassengerSeatState = FrontPassengerSeatState::vacated;
                }
                break;
            case EventType::frontPassengerSideSeatOccupationStatus:
                if (_storedReportedStatuses.frontPassengerSeatIsOccupied)
                {
                    StartTimer(Timers::frontPassengerSideSeatOccupationStatusStale, TimerPeriod::frontPassengerSideSeatOccupationStatusStale);
                    _frontPassengerSeatState = FrontPassengerSeatState::occupied;
                }
                else
                {
                    _frontPassengerSeatState = FrontPassengerSeatState::vacated;
                }
                break;
            default:
                break;
            }
            break;
        }
        if (_frontPassengerSeatState != previousFrontPassengerSeatState)
        {
            frontPassengerSeatStateChanged = true;
            Trace("Changing _frontPassengerSeatState from %s to %s", ToString(previousFrontPassengerSeatState), ToString(_frontPassengerSeatState));
        }

        // WAL can be cancelled before and during execution
        // Before execution it can be cancelled by the remote control and handbrake toggle, but not
        // by the door handle buttons
        // During execution the closing of windows and roof can be stopped by both remote control
        // and door handle buttons
        // During the first few seconds of execution a button press will also cancel WAL to make it
        // possible for the user to cancel if they forgot to do it beforehand
        // After that period a button press will only stop the motion but WAL will lock the car
        // again under the right conditions
        // The cancellation lasts until all doors are closed and locked, or until the car has moved
        //
        // Remote control considerations:
        //   When WAL is not being executed:
        //     When locking the car with the lock button there is no need for cancellation
        //     If the driver door is open a press of the lock button will cancel WAL
        //     If we unlock the car with the remote control we want WAL to be armed
        //     In that case the car will lock again automatically if we don't open a door
        //     From the time we open a door it is possible to cancel WAL
        //     Then a press of the unlock button will cancel WAL even if a door is open
        //   When WAL is being executed:
        //     A press of the lock or unlock button will cancel execution
        //     If pressed during the first few seconds of execution it will also cancel WAL as
        //     described
        //
        // Door handle button considerations:
        //   When WAL is not being executed:
        //     When locking the car with the door handle button there is no need for cancellation
        //     If we unlock the car with the door handle button we want WAL to be armed
        //     In that case the car will lock again automatically if we don't open a door
        //     When the car is unlocked, we don't receive any door handle button events anymore
        //     except when using it to lock the car
        //     This renders the door handle buttons useless for WAL cancellation in case the car is
        //     unlocked
        //   When WAL is being executed:
        //     A press of the door handle button will cancel execution
        //     If pressed during the first few seconds of execution it will also cancel WAL as
        //     described
        //
        // About handbrake toggle:
        //   By toggling the handbrake on/off or off/on quickly, WAL can be cancelled before
        //   exiting the car
        // 
        // Summary:
        // Unlock car with remote control or door handle button -> doors remain closed -> automatically lock
        // Unlock car -> door opened -> cancellation possible by remote control and handbrake toggle -> situation (A)
        //   Situation (A) -> door closed -> executing WAL -> cancellation possible by all means
        //   Situation (A) -> WAL cancelled -> car locked by user / car is being driven -> new WAL possible
        auto previousWalCancelState = _walCancelState;
        bool walCancelStateChanged = false;
        bool walCancelledDuringExecutionTrigger = false;
        switch (_walCancelState)
        {
        case WalCancelState::lockedOrNoDoorOpenedAfterUnlock:
            // This state will also be entered when the car is driven and the doors are automatically locked
            switch (event.type)
            {
            // Timer for test code
            // case EventType::timerExpiry:
            //     _iSound->IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound::goFast);
            //     StartTimer(Timers::intermediateCountdownSoundInterval, TimerPeriod::intermediateCountdownSoundInterval);
            //     break;
            case EventType::doorOpenStatus:
                if (!DoorsBootAndBonnetAreClosed())
                {
                    _walCancelState = WalCancelState::cancellationBeforeExecutionPossible;
                }
                break;
            case EventType::remoteControlButtonPressed:
            case EventType::doorHandleButtonPressed:
                // At any time during execution WAL can be cancelled
                if (_mainWalState == MainWalState::executing)
                {
                    if (TimerRunning(Timers::permanentCancelPeriodDuringWalExecution))
                    {
                        _walCancelState = WalCancelState::walCancelled;
                    }
                    walCancelledDuringExecutionTrigger = true;
                    Trace("Cancelled during execution trigger is set");
                }
                break;
            case EventType::handbrakeToggleRepeat:
                _walCancelState = WalCancelState::walCancelled;
                _iSound->IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound::intermediateCountdown);
                break;
            default:
                break;
            }
            break;
        case WalCancelState::cancellationBeforeExecutionPossible:
            switch (event.type)
            {
            case EventType::doorOpenStatus:
            case EventType::doorLockStatus:
                if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
                {
                    _walCancelState = WalCancelState::lockedOrNoDoorOpenedAfterUnlock;
                }
                break;
            case EventType::remoteControlButtonPressed:
            case EventType::handbrakeToggleRepeat:
                _walCancelState = WalCancelState::walCancelled;
                _iSound->IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound::intermediateCountdown);
                break;
            default:
                break;
            }
            break;
        case WalCancelState::walCancelled:
            switch (event.type)
            {
            case EventType::doorOpenStatus:
            case EventType::doorLockStatus:
                if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
                {
                    _walCancelState = WalCancelState::lockedOrNoDoorOpenedAfterUnlock;
                }
                break;
            case EventType::vehicleSpeed:
                if (_storedReportedStatuses.vehicleSpeed > _drivingSpeedThreshold)
                {
                    _walCancelState = WalCancelState::lockedOrNoDoorOpenedAfterUnlock;
                }
                break;
            case EventType::handbrakeToggleRepeat:
                _iSound->IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound::goFast);
                break;
            default:
                break;
            }
            break;
        }
        if (_walCancelState != previousWalCancelState)
        {
            walCancelStateChanged = true;
            Trace("Changing _walCancelState from %s to %s", ToString(previousWalCancelState), ToString(_walCancelState));
        }

        auto previousDoorOpenSequenceAfterUnlockState = _doorOpenSequenceAfterUnlockState;
        switch (_doorOpenSequenceAfterUnlockState)
        {
        case DoorOpenSequenceAfterUnlockState::lockedOrNoDoorOpenedAfterUnlock:
            // This state will also be entered when the car is driven and the doors are automatically locked
            switch (event.type)
            {
            case EventType::doorOpenStatus:
                if (_storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen)
                {
                    _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::driverDoorOpenedAfterUnlock;
                }
                else
                {
                    if (
                        _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen ||
                        _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen ||
                        _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen
                        )
                    {
                        _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::onlyNonDriverDoorOpenedAfterUnlock;
                    }

                }
                break;
            default:
                break;
            }
            break;
        case DoorOpenSequenceAfterUnlockState::driverDoorOpenedAfterUnlock:
            switch (event.type)
            {
            case EventType::doorOpenStatus:
            case EventType::doorLockStatus:
                if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
                {
                    _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::lockedOrNoDoorOpenedAfterUnlock;
                }
                break;
            default:
                break;
            }
            break;
        case DoorOpenSequenceAfterUnlockState::onlyNonDriverDoorOpenedAfterUnlock:
            switch (event.type)
            {
            case EventType::doorOpenStatus:
                if (_storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen)
                {
                    _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::driverDoorOpenedAfterUnlock;
                }
                else
                {
                    if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
                    {
                        _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::lockedOrNoDoorOpenedAfterUnlock;
                    }
                }
                break;
            case EventType::doorLockStatus:
                if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
                {
                    _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::lockedOrNoDoorOpenedAfterUnlock;
                }
                break;
            default:
                break;
            }
            break;
        }
        if (_doorOpenSequenceAfterUnlockState != previousDoorOpenSequenceAfterUnlockState)
        {
            Trace("Changing _doorOpenSequenceAfterUnlockState from %s to %s", ToString(previousDoorOpenSequenceAfterUnlockState), ToString(_doorOpenSequenceAfterUnlockState));
        }

        // The main WAL state machine waits for all conditions to become "go"
        // At that point it waits a certain period before actually locking the car
        // The execution of locking the car takes some time after which we can be certain that the
        // received statuses are "no go" and we can return to the initial state
        // At all times during "go" or "executing" the process can be stopped via the remote control
        // which is monitored by the WAL cancel state machine
        auto previousMainWalState = _mainWalState;
        switch (_mainWalState)
        {
        case MainWalState::noGo:
            // EventForGoCondition will check the event type for us
            if (EventForGoCondition(event.type, frontPassengerSeatStateChanged, walCancelStateChanged))
            {
                if (GoConditionActive())
                {
                    if (_doorOpenSequenceAfterUnlockState == DoorOpenSequenceAfterUnlockState::onlyNonDriverDoorOpenedAfterUnlock)
                    {
                        //**********************
                        // Start goSlow/intermediateCountdown sound generation
                        //**********************
                        StartTimer(Timers::lockCarWait, TimerPeriod::lockCarWaitLong);
                    }
                    else
                    {
                        //**********************
                        // Start goFast/intermediateCountdown sound generation
                        //**********************
                        _iSound->IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound::goFast);
                        StartTimer(Timers::lockCarWait, TimerPeriod::lockCarWaitShort);
                    }
                    _mainWalState = MainWalState::go;
                }
            }
            break;
        case MainWalState::go:
            switch (event.type)
            {
            case EventType::timerExpiry:
                switch (event.timerExpiry.timerId)
                {
                case Timers::lockCarWait:
                    if (_doorOpenSequenceAfterUnlockState == DoorOpenSequenceAfterUnlockState::driverDoorOpenedAfterUnlock)
                    {
                        bool mayCloseWindowsAndRoof = !_storedReportedStatuses.mirrorsAreFolded;
                        if (mayCloseWindowsAndRoof != _storeValueMayCloseWindowsAndRoof)
                        {
                            _storeValueMayCloseWindowsAndRoof = mayCloseWindowsAndRoof;
                            Trace("Setting _storeValueMayCloseWindowsAndRoof(%s)", _storeValueMayCloseWindowsAndRoof ? "true" : "false");
                            _iStore->IPiStoreSetMayCloseWindowsAndRoof(_storeValueMayCloseWindowsAndRoof);
                        }
                    }
                    LockDoors();
                    if (_storeValueMayCloseWindowsAndRoof)
                    {
                        StartTimer(Timers::foldMirrorsCanMessageWait, TimerPeriod::foldMirrorsCanMessageWait);
                        StartTimer(Timers::stopWalExecution, TimerPeriod::stopWalExecutionAfterClosingWindowsAndRoof);
                    }
                    else
                    {
                        StartTimer(Timers::foldMirrorsCanMessageWait, TimerPeriod::foldMirrorsCanMessageWait);
                        StartTimer(Timers::stopWalExecution, TimerPeriod::stopWalExecutionAfterLocking);
                    }
                    StartTimer(Timers::permanentCancelPeriodDuringWalExecution, TimerPeriod::permanentCancelPeriodDuringWalExecution);
                    _mainWalState = MainWalState::executing;
                    break;
                case Timers::intermediateCountdownSoundInterval:
                    break;
                default:
                    break;
                }
                break;
            default:
                // EventForGoCondition will check the event type for us
                if (EventForGoCondition(event.type, frontPassengerSeatStateChanged, walCancelStateChanged))
                {
                    if (!GoConditionActive())
                    {
                        StopTimer(Timers::lockCarWait);
                        _mainWalState = MainWalState::noGo;
                    }
                }
                break;
            }
            break;
        case MainWalState::executing:
            switch (event.type)
            {
            case EventType::timerExpiry:
                switch (event.timerExpiry.timerId)
                {
                case Timers::foldMirrorsCanMessageWait:
                    SendFoldMirrorsMessage();
                    if (_storeValueMayCloseWindowsAndRoof)
                    {
                        StartTimer(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait, TimerPeriod::closeWindowsAndRoofAndFoldMirrorsCanMessageInterval);
                    }
                    break;
                case Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait:
                    SendCloseWindowsAndRoofAndFoldMirrorsMessage();
                    StartTimer(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait, TimerPeriod::closeWindowsAndRoofAndFoldMirrorsCanMessageInterval);
                    break;
                case Timers::stopWalExecution:
                    if (TimerRunning(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait)) StopTimer(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait);
                    if (TimerRunning(Timers::permanentCancelPeriodDuringWalExecution)) StopTimer(Timers::permanentCancelPeriodDuringWalExecution);
                    _mainWalState = MainWalState::noGo;
                    break;
                default:
                    break;
                }
                break;
            case EventType::windowRoofAndMirrorControl:
                if (_storeValueMayCloseWindowsAndRoof && TimerRunning(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait))
                {
                    // Another ECU might send this message, thereby stopping the movement of windows and roof
                    // To minimize that effect introduce a delay before continuing to send messages
                    StopTimer(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait);
                    StartTimer(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait, TimerPeriod::closeWindowsAndRoofAndFoldMirrorsCanMessageWaitAfterStop);
                }
                break;
            default:
                // Only an active cancellation may stop us now
                if (walCancelledDuringExecutionTrigger)
                {
                    if (TimerRunning(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait))
                    {
                        StopTimer(Timers::closeWindowsAndRoofAndFoldMirrorsCanMessageWait);
                        SendStopCloseWindowsAndRoofAndFoldMirrorsMessage();
                    }
                    StopTimer(Timers::stopWalExecution);
                    if (TimerRunning(Timers::permanentCancelPeriodDuringWalExecution)) StopTimer(Timers::permanentCancelPeriodDuringWalExecution);
                    _mainWalState = MainWalState::noGo;
                }
                break;
            }
            break;
        }
        if (_mainWalState != previousMainWalState)
        {
            Trace("Changing _mainWalState from %s to %s", ToString(previousMainWalState), ToString(_mainWalState));
        }
    }

    void CanWalProcessing::TraceEvent(Event event)
    {
        switch (event.type)
        {
        case EventType::timerExpiry:
            Trace("Event timerExpiry(%s)", event.timerExpiry.timerName);
            break;
        case EventType::frontPassengerSideSeatOccupationStatus:
            Trace("Event frontPassengerSideSeatOccupationStatus(%d)", _storedReportedStatuses.frontPassengerSeatIsOccupied);
            break;
        case EventType::doorOpenStatus:
            Trace(
                "Event doorOpenStatus(%d,%d,%d,%d,%d,%d)",
                _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen,
                _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen,
                _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen,
                _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen,
                _storedReportedStatuses.doorOpenStatuses.bootIsOpen,
                _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen
            );
            break;
        case EventType::doorLockStatus:
            Trace(
                "Event doorLockStatus(%d,%d,%d,%d)",
                _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked,
                _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked,
                _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked,
                _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked
            );
            break;
        case EventType::remoteControlButtonPressed:
            Trace("Event remoteControlButtonPressed");
            break;
        case EventType::doorHandleButtonPressed:
            Trace("Event doorHandleButtonPressed");
            break;
        case EventType::vehicleSpeed:
            Trace("Event vehicleSpeed(%d)", _storedReportedStatuses.vehicleSpeed);
            break;
        case EventType::keyLocation:
            Trace("Event keyLocation(%d)", _storedReportedStatuses.keyIsOutside);
            break;
        case EventType::windowRoofAndMirrorControl:
            //_iCanWalProcessingOs->Trace("Event windowRoofAndMirrorControl");
            break;
        case EventType::handbrakeToggleRepeat:
            Trace("Event handbrakeToggleRepeat");
            break;
        default:
            break;
        }
    }

    bool CanWalProcessing::DoorsBootAndBonnetAreClosed()
    {
        return
            !_storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen &&
            !_storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen &&
            !_storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen &&
            !_storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen &&
            !_storedReportedStatuses.doorOpenStatuses.bootIsOpen &&
            !_storedReportedStatuses.doorOpenStatuses.bonnetIsOpen;
    }

    bool CanWalProcessing::AllDoorsAreLocked()
    {
        return
            _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked &&
            _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked &&
            _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked &&
            _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked;
    }

    bool CanWalProcessing::EventForGoCondition(EventType eventType, bool frontPassengerSeatStateChanged, bool walCancelStateChanged)
    {
        // Besides the event type we are also interested in state changes of other state machines
        return
            eventType == EventType::doorOpenStatus ||
            eventType == EventType::doorLockStatus ||
            eventType == EventType::keyLocation ||
            frontPassengerSeatStateChanged ||
            walCancelStateChanged;
    }

    bool CanWalProcessing::GoConditionActive()
    {
        return
            _storedReportedStatuses.keyIsOutside &&
            _frontPassengerSeatState == FrontPassengerSeatState::vacated &&
            _walCancelState != WalCancelState::walCancelled &&
            DoorsBootAndBonnetAreClosed() &&
            !AllDoorsAreLocked() &&
            _storeValueEnableWal;
    }

    void CanWalProcessing::LockDoors()
    {
        if (_useRemoteControl)
        {
            if (_storeValueMayCloseWindowsAndRoof)
            {
                _iRemoteControl->IPiRemoteControlLockSinglePress();
            }
            else
            {
                _iRemoteControl->IPiRemoteControlLockDoublePress();
            }
            _performingRemoteControlOperation = true;
        }
        else
        {
            SendLockDoorsMessage();
        }
    }

    void CanWalProcessing::SendLockDoorsMessage()
    {
        unsigned char data[8] = { 0x33, 0x33, 0x38, 0x00 };
        for (int index = 0; index < 4; index++)
        {
            data[index + 4] = _storedReportedStatuses.doorLockControlLast4Bytes[index];
        }
        _iCan->IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId::doorLockControl, sizeof(data), data);
    }

    void CanWalProcessing::SendFoldMirrorsMessage()
    {
        constexpr unsigned char data[8] = { 0x00, 0x1B, 0x00, 0x52, 0xFF, 0xFF, 0xFF, 0xFF };
        _iCan->IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId::windowRoofAndMirrorControl, sizeof(data), data);
    }

    void CanWalProcessing::SendCloseWindowsAndRoofAndFoldMirrorsMessage()
    {
        constexpr unsigned char data[8] = { 0x1B, 0x1B, 0x1B, 0x52, 0xFF, 0xFF, 0xFF, 0xFF };
        _iCan->IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId::windowRoofAndMirrorControl, sizeof(data), data);
    }

    void CanWalProcessing::SendStopCloseWindowsAndRoofAndFoldMirrorsMessage()
    {
        constexpr unsigned char data[8] = { 0x00, 0x00, 0x00, 0x50, 0xFF, 0xFF, 0xFF, 0xFF };
        _iCan->IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId::windowRoofAndMirrorControl, sizeof(data), data);
    }

    void CanWalProcessing::StartTimer(int timerId, TimerPeriod timerPeriod)
    {
        PlatformIndependent::Commons::Os::StartTimer(timerId, (int)timerPeriod);
    }
}