#include "CanWalProcessing/CanWalProcessing.h"

CanWalProcessing::CanWalProcessing()
{
        _storedReportedStatuses.doorOpenStatuses.frontDriverSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.rearDriverSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.frontPassengerSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.rearPassengerSideDoorIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.bootIsOpen = false;
        _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen = false;
        _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked = false;
        _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked = false;
        _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked = false;
        _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked = false;
        _storedReportedStatuses.keyIsOutside = false;
        _storedReportedStatuses.mirrorsAreFolded = false;
        for (int index = 0; index < 4; index++)
        {
            _storedReportedStatuses.doorLockControlLast4Bytes[index] = 0;
        }
        _frontPassengerSeatState = FrontPassengerSeatState::vacated;
        _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancellationNotPossible;
        _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock;
        _mainWalState = MainWalState::noGo;
}

void CanWalProcessing::SetOutputInterfaces(ICanWalProcessing::Os::Output* iOs, ICanWalProcessing::Can::Output* iCan, ICanWalProcessing::Sound::Output* iSound)
{
    _iOs = iOs;
    _iCan = iCan;
    _iSound = iSound;
}

void CanWalProcessing::HandleExpiredTimer(int timerId)
{
    Timer::FindTimer(timerId)->_running = false;
    Event event;
    event.type = EventType::timerExpiry;
    event.timerExpiry.timerId = timerId;
    HandleEvent(event);
}

void CanWalProcessing::HandleCanMessage(int id, int length, const unsigned char* data)
{
    switch ((CanId)id)
    {
    case CanId::frontPassengerSideDoorStatus:
    {
        // The open status is in bit 0 of byte 3
        bool doorIsOpen = (data[3] & 0x01) == 1;
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
    case CanId::rearPassengerSideDoorStatus:
    {
        // The open status is in bit 0 of byte 3
        bool doorIsOpen = (data[3] & 0x01) == 1;
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
    case CanId::frontDriverSideDoorStatus:
    {
        // The open status is in bit 0 of byte 3
        bool doorIsOpen = (data[3] & 0x01) == 1;
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
    case CanId::rearDriverSideDoorStatus:
    {
        // The open status is in bit 0 of byte 3
        bool doorIsOpen = (data[3] & 0x01) == 1;
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
    case CanId::mirrorFoldStatus:
    {
        // Byte 0 will be F7 when the mirrors are being folded
        bool mirrorsAreFolded = data[0] == 0xF7;
        if (mirrorsAreFolded != _storedReportedStatuses.mirrorsAreFolded)
        {
            _storedReportedStatuses.mirrorsAreFolded = mirrorsAreFolded;
            // This only needs to be stored, no event handling needed
            _iOs->Trace("Status mirrorFoldStatus(%d)", mirrorsAreFolded);
        }
    }
    break;
    case CanId::ignitionAndKeyStatus:
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
    case CanId::vehicleSpeed:
    {
        // The vehicle speed is in the first 12 bits of bytes 0 and 1
        // The unit is 0.1 km/h
        int vehicleSpeed = (data[0] + ((data[1] & 0x0F) << 16)) / 10;
        if (vehicleSpeed != _storedReportedStatuses.vehicleSpeed)
        {
            _storedReportedStatuses.vehicleSpeed = vehicleSpeed;
            Event event;
            event.type = EventType::vehicleSpeed;
            HandleEvent(event);
        }
    }
    break;
    case CanId::remoteControlAndDoorHandleInput:
    {
        // Bit 0 of byte 2 represents the unlock button
        // Bit 2 of byte 2 represents the lock button
        if ((data[2] & 0x01) == 1 || ((data[2] & 0x04) >> 2) == 1)
        {
            Event event;
            // Bits 0 and 1 of byte 1 are zero when the input comes from the remote control
            // They are 3 when it comes from the door handle
            if ((data[1] & 0x03) == 0)
            {
                event.type = EventType::remoteControlButtonPressed;
            }
            else
            {
                event.type = EventType::doorHandleButtonPressed;
            }
            HandleEvent(event);
        }
    }
    break;
    case CanId::windowRoofAndMirrorControl:
    {
        Event event;
        event.type = EventType::windowRoofAndMirrorControl;
        HandleEvent(event);
    }
    break;
    case CanId::doorLockControl:
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
    case CanId::dateTime:
    {
        Event event;
        event.type = EventType::dateTime;
        event.dateTime.day = data[3] + 1;
        event.dateTime.month = data[4] >> 4;
        event.dateTime.year = data[5] + (data[6] << 8);
        event.dateTime.hour = data[0];
        event.dateTime.minute = data[1];
        event.dateTime.second = data[2];
        HandleEvent(event);
    }
    break;
    case CanId::seatbeltAndSeatOccupancyStatus:
    {
        bool seatIsOccupied = data[1] != 0x08;
        if (seatIsOccupied != _storedReportedStatuses.frontPassengerSeatIsOccupied)
        {
            _storedReportedStatuses.frontPassengerSeatIsOccupied = seatIsOccupied;
            Event event;
            event.type = EventType::frontPassengerSeatOccupationStatus;
            HandleEvent(event);
        }
    }
    break;
    case CanId::doorStatus:
    {
        bool handleDoorOpenStatusEvent = false;
        // The boot open status is in bit 0 of byte 2
        bool bootIsOpen = (data[2] & 0x01) == 1;
        if (bootIsOpen != _storedReportedStatuses.doorOpenStatuses.bootIsOpen)
        {
            _storedReportedStatuses.doorOpenStatuses.bootIsOpen = bootIsOpen;
            handleDoorOpenStatusEvent = true;
        }
        // The bonnet open status is in bit 2 of byte 2
        bool bonnetIsOpen = ((data[2] & 0x04) >> 2) == 1;
        if (bonnetIsOpen != _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen)
        {
            _storedReportedStatuses.doorOpenStatuses.bonnetIsOpen = bonnetIsOpen;
            handleDoorOpenStatusEvent = true;
        }
        if (handleDoorOpenStatusEvent)
        {
            Event event;
            event.type = EventType::doorOpenStatus;
            HandleEvent(event);
        }
    }
    break;
    default:
        break;
    }
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
    case WalCancelState::noDoorOpenedAfterUnlockCancellationNotPossible: return "noDoorOpenedAfterUnlockCancellationNotPossible";
    case WalCancelState::doorOpenedAfterUnlockCancellationPossible: return "doorOpenedAfterUnlockCancellationPossible";
    case WalCancelState::walCancelled: return "walCancelled";
    default: return "";
    }
}

const char* CanWalProcessing::ToString(DoorOpenSequenceAfterUnlockState doorOpenSequenceAfterUnlockState)
{
    switch (doorOpenSequenceAfterUnlockState)
    {
    case DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock: return "noDoorOpenedAfterUnlock";
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
        case EventType::frontPassengerSeatOccupationStatus:
            if (_storedReportedStatuses.frontPassengerSeatIsOccupied)
            {
                StartTimer(Timers::frontPassengerSeatOccupationStatusStale, TimerPeriod::frontPassengerSeatOccupationStatusStale);
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
        case EventType::frontPassengerSeatOccupationStatus:
            if (_storedReportedStatuses.frontPassengerSeatIsOccupied)
            {
                // Every time we receive a new occupied status the timer must be restarted
                StartTimer(Timers::frontPassengerSeatOccupationStatusStale, TimerPeriod::frontPassengerSeatOccupationStatusStale);
            }
            else
            {
                StopTimer(Timers::frontPassengerSeatOccupationStatusStale);
                _frontPassengerSeatState = FrontPassengerSeatState::vacated;
            }
            break;
        case EventType::timerExpiry:
            // When the timer expires the occupied status becomes stale
            if (event.timerExpiry.timerId == Timers::frontPassengerSeatOccupationStatusStale)
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
        case EventType::frontPassengerSeatOccupationStatus:
            if (_storedReportedStatuses.frontPassengerSeatIsOccupied)
            {
                StartTimer(Timers::frontPassengerSeatOccupationStatusStale, TimerPeriod::frontPassengerSeatOccupationStatusStale);
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
        _iOs->Trace("Changing _frontPassengerSeatState from %s to %s", ToString(previousFrontPassengerSeatState), ToString(_frontPassengerSeatState));
    }

    // WAL can be cancelled by the remote control
    // During execution it can also be cancelled by the door handle buttons to stop
    // the windows and roof being closed
    // If we unlock the car with the remote control without opening a door afterwards
    // we want it to close again automatically so in that case WAL must not be cancelled
    // Therefore the cancel action is only allowed after the car in unlocked and a door
    // has been opened
    // When a door is opened with the door handle buttons we will receive events but it
    // is not the user's intent to cancel WAL
    // For that reason door handle button presses may only cancel WAL during execution
    // The cancellation lasts until all doors are closed and locked, or until the car has moved
    // Summary:
    // Unlock car -> door opened -> cancellation possible by remote control
    // (A) -> door closed -> executing WAL -> cancellation possible by both remote control and door handle buttons
    // (B) -> WAL cancelled -> car locked by user / car is being driven -> new WAL possible
    auto previousWalCancelState = _walCancelState;
    bool walCancelStateChanged = false;
    switch (_walCancelState)
    {
    case WalCancelState::noDoorOpenedAfterUnlockCancellationNotPossible:
        switch (event.type)
        {
        case EventType::doorOpenStatus:
            if (!DoorsBootAndBonnetAreClosed())
            {
                _walCancelState = WalCancelState::doorOpenedAfterUnlockCancellationPossible;
            }
            break;
        case EventType::remoteControlButtonPressed:
        case EventType::doorHandleButtonPressed:
            //if (!_remoteControlIsBeingUsed)
            // At any time during execution WAL can be cancelled
            // In all other situations we must wait until a doors opens before we may cancel
            if (_mainWalState == MainWalState::executing)
            {
                _walCancelState = WalCancelState::walCancelled;
            }
            break;
        default:
            break;
        }
        break;
    case WalCancelState::doorOpenedAfterUnlockCancellationPossible:
        switch (event.type)
        {
        case EventType::doorOpenStatus:
        case EventType::doorLockStatus:
            if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
            {
                _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancellationNotPossible;
            }
            break;
        case EventType::remoteControlButtonPressed:
            //if (!_remoteControlIsBeingUsed)
            {
                _walCancelState = WalCancelState::walCancelled;
            }
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
                _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancellationNotPossible;
            }
            break;
        case EventType::vehicleSpeed:
            if (_storedReportedStatuses.vehicleSpeed > 0)
            {
                _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancellationNotPossible;
            }
            break;
        default:
            break;
        }
        break;
    }
    if (_walCancelState != previousWalCancelState)
    {
        walCancelStateChanged = true;
        _iOs->Trace("Changing _walCancelState from %s to %s", ToString(previousWalCancelState), ToString(_walCancelState));
    }

    auto previousDoorOpenSequenceAfterUnlockState = _doorOpenSequenceAfterUnlockState;
    switch (_doorOpenSequenceAfterUnlockState)
    {
    case DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock:
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
                _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock;
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
                    _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock;
                }
            }
            break;
        case EventType::doorLockStatus:
            if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked())
            {
                _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock;
            }
            break;
        default:
            break;
        }
        break;
    }
    if (_doorOpenSequenceAfterUnlockState != previousDoorOpenSequenceAfterUnlockState)
    {
        _iOs->Trace("Changing _doorOpenSequenceAfterUnlockState from %s to %s", ToString(previousDoorOpenSequenceAfterUnlockState), ToString(_doorOpenSequenceAfterUnlockState));
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
                SendLockDoorsMessage();
                if (_storedReportedStatuses.mirrorsAreFolded)
                {
                    StartTimer(Timers::stopWalExecution, TimerPeriod::stopWalExecutionShort);
                }
                else
                {
                    SendCloseWindowsAndRoofAndFoldMirrorsMessage();
                    StartTimer(Timers::closeWindowsAndRoofCanMessageInterval, TimerPeriod::closeWindowsAndRoofCanMessageInterval);
                    StartTimer(Timers::stopWalExecution, TimerPeriod::stopWalExecutionLong);
                }
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
            case Timers::closeWindowsAndRoofCanMessageInterval:
                SendCloseWindowsAndRoofAndFoldMirrorsMessage();
                StartTimer(Timers::closeWindowsAndRoofCanMessageInterval, TimerPeriod::closeWindowsAndRoofCanMessageInterval);
                break;
            case Timers::stopWalExecution:
                if (TimerRunning(Timers::closeWindowsAndRoofCanMessageInterval))
                {
                    StopTimer(Timers::closeWindowsAndRoofCanMessageInterval);
                }
                _mainWalState = MainWalState::noGo;
                break;
            default:
                break;
            }
            break;
        case EventType::windowRoofAndMirrorControl:
            // Another ECU might send this message, thereby stopping the movement of windows and roof
            // To minimize that effect send a close message now
            SendCloseWindowsAndRoofAndFoldMirrorsMessage();
            break;
        default:
            // Only an active cancellation may stop us now
            if (_walCancelState == WalCancelState::walCancelled)
            {
                if (TimerRunning(Timers::closeWindowsAndRoofCanMessageInterval))
                {
                    StopTimer(Timers::closeWindowsAndRoofCanMessageInterval);
                }
                StopTimer(Timers::stopWalExecution);
                _mainWalState = MainWalState::noGo;
            }
            break;
        }
        break;
    }
    if (_mainWalState != previousMainWalState)
    {
        _iOs->Trace("Changing _mainWalState from %s to %s", ToString(previousMainWalState), ToString(_mainWalState));
    }
}

void CanWalProcessing::TraceEvent(Event event)
{
    switch (event.type)
    {
    case EventType::timerExpiry:
        _iOs->Trace("Event timerExpiry(%s)", Timer::FindTimer(event.timerExpiry.timerId)->_name);
        break;
    case EventType::frontPassengerSeatOccupationStatus:
        _iOs->Trace("Event frontPassengerSeatOccupationStatus(%d)", _storedReportedStatuses.frontPassengerSeatIsOccupied);
        break;
    case EventType::doorOpenStatus:
        _iOs->Trace(
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
        _iOs->Trace("Event doorLockStatus(%d,%d,%d,%d)",
            _storedReportedStatuses.doorLockStatuses.frontDriverSideDoorIsLocked,
            _storedReportedStatuses.doorLockStatuses.rearDriverSideDoorIsLocked,
            _storedReportedStatuses.doorLockStatuses.frontPassengerSideDoorIsLocked,
            _storedReportedStatuses.doorLockStatuses.rearPassengerSideDoorIsLocked
        );
        break;
    case EventType::remoteControlButtonPressed:
        _iOs->Trace("Event remoteControlButtonPressed");
        break;
    case EventType::doorHandleButtonPressed:
        _iOs->Trace("Event doorHandleButtonPressed");
        break;
    case EventType::vehicleSpeed:
        _iOs->Trace("Event vehicleSpeed(%d)",
            _storedReportedStatuses.vehicleSpeed
        );
        break;
    case EventType::keyLocation:
        _iOs->Trace("Event keyLocation(%d)",
            _storedReportedStatuses.keyIsOutside
        );
        break;
    case EventType::windowRoofAndMirrorControl:
        //_iCanWalProcessingOs->Trace("Event windowRoofAndMirrorControl");
        break;
    case EventType::dateTime:
        _iOs->Trace("Event dateTime(%d-%02d-%02d %02d:%02d:%02d)",
            event.dateTime.year,
            event.dateTime.month,
            event.dateTime.day,
            event.dateTime.hour,
            event.dateTime.minute,
            event.dateTime.second
            );
        break;
    default:
        break;
    }
}

void CanWalProcessing::StartTimer(int timerId, TimerPeriod timerPeriod)
{
    Timer* timer = Timer::FindTimer(timerId);
    if (timer->_running)
    {
        _iOs->StopTimer(timerId);
    }
    else
    {
        timer->_running = true;
    }

    _iOs->Trace("Starting timer \"%s\" with a period of %d ms", timer->_name, timerPeriod);
    _iOs->StartTimer(timerId, (int)timerPeriod);
}

void CanWalProcessing::StopTimer(int timerId)
{
    Timer* timer = Timer::FindTimer(timerId);
    if (timer->_running)
    {
        _iOs->Trace("Stopping timer \"%s\"", timer->_name);
        _iOs->StopTimer(timerId);
        timer->_running = false;
    }
}

bool CanWalProcessing::TimerRunning(int timerId)
{
    return Timer::FindTimer(timerId)->_running;
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
        !AllDoorsAreLocked();
}

void CanWalProcessing::SendLockDoorsMessage()
{
    unsigned char data[8] = { 0x33, 0x33, 0x38, 0x00 };
    for (int index = 0; index < 4; index++)
    {
        data[index + 4] = _storedReportedStatuses.doorLockControlLast4Bytes[index];
    }
    _iCan->SendCanMessage((int)CanId::doorLockControl, sizeof(data), data);
}

void CanWalProcessing::SendCloseWindowsAndRoofAndFoldMirrorsMessage()
{
    unsigned char data[8] = { 0x1B, 0x1B, 0x1B, 0x52, 0xFF, 0xFF, 0xFF, 0xFF };
    _iCan->SendCanMessage(int(CanId::windowRoofAndMirrorControl), sizeof(data), data);
}

CanWalProcessing::Timer* CanWalProcessing::Timer::_lastTimer = nullptr;

CanWalProcessing::Timer::Timer(int timerId, const char* name) :
    _timerId(timerId),
    _name(name),
    _previousTimer(_lastTimer)
{
    _running = false;
    _lastTimer = this;
}

CanWalProcessing::Timer* CanWalProcessing::Timer::FindTimer(int timerId)
{
    Timer* timer = Timer::_lastTimer;
    do
    {
        if (timer->_timerId == timerId)
        {
            return timer;
        }
        timer = timer->_previousTimer;
    } while (timer != nullptr);
    return nullptr;
}
