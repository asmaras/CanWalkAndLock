#include "CanWalProcessing.h"

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
        _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancelNotPossible;
        _doorOpenSequenceAfterUnlockState = DoorOpenSequenceAfterUnlockState::noDoorOpenedAfterUnlock;
        _mainWalState = MainWalState::noGo;
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
        _iCanWalProcessingOut->Trace("doorIsOpen=%d", doorIsOpen);
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
        _storedReportedStatuses.mirrorsAreFolded = data[0] == 0xF7;
    }
    break;

    case CanId::ignitionAndKeyStatus:
    {
        bool keyIsOutside = data[1] == 0x0E && data[3] == 0x04;
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

    case CanId::remoteControlInput:
    {
        switch (data[2])
        {
        case 0x01:
        {
            Event event;
            event.type = EventType::remoteControlUnlockButtonPressed;
            HandleEvent(event);
        }
        break;
        case 0x04:
        {
            Event event;
            event.type = EventType::remoteControlLockButtonPressed;
            HandleEvent(event);
        }
        break;
        }
    }
    break;
    
    case CanId::doorLockControl:
    {
        if (length == 8)
        {
            for (int index = 0; index < 4; index++)
            {
                _storedReportedStatuses.doorLockControlLast4Bytes[index] = data[index + 4];
            }
        }
    }
    break;
    
    case CanId::dateTime:
    {
        Event event;
        event.type = EventType::dateTime;
        event.dateTime.day = data[3] + 1;
        event.dateTime.month = data[4];
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
        bool bonnetIsOpen = (data[2] & 0x04) >> 2 == 1;
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

    // Timer* timer = Timer::_lastTimer;
    // do
    // {
    //     _iCanWalProcessingOut->Trace("Found timer %d", timer->_timerId);
    //     timer = timer->_previousTimer;
    // } while (timer != nullptr);
    // StartTimer(Timers::intermediateCountdownSoundInterval, TimerPeriod::intermediateCountdownSoundInterval);
}

void CanWalProcessing::HandleExpiredTimer(int timerId)
{
    Timer* timer = Timer::FindTimer(timerId);
    timer->_running = false;
    _iCanWalProcessingOut->Trace("Timer \"%s\" expired", timer->_name);
    Event event;
    event.type = EventType::timerExpiry;
    event.timerExpiry.timerId = timerId;
    HandleEvent(event);
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
    case WalCancelState::noDoorOpenedAfterUnlockCancelNotPossible: return "noDoorOpenedAfterUnlockCancelNotPossible";
    case WalCancelState::doorOpenedAfterUnlockCancelPossible: return "doorOpenedAfterUnlockCancelPossible";
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
        _iCanWalProcessingOut->Trace("Changing _frontPassengerSeatState from %s to %s", ToString(previousFrontPassengerSeatState), ToString(_frontPassengerSeatState));
    }

    // WAL can be cancelled by the remote control
    // However if we unlock the car with the remote control without opening a door afterwards
    // we want it to close again automatically so in that case WAL must not be cancelled. Therefore
    // the cancel action is only allowed after a door has been opened.
    // The cancellation lasts until all doors are closed and locked, or until the car has moved.
    auto previousWalCancelState = _walCancelState;
    bool walCancelStateChanged = false;
    switch (_walCancelState)
    {
    case WalCancelState::noDoorOpenedAfterUnlockCancelNotPossible:
        switch (event.type)
        {
        case EventType::doorOpenStatus:
            if (!DoorsBootAndBonnetAreClosed())
            {
                _walCancelState = WalCancelState::doorOpenedAfterUnlockCancelPossible;
            }
            break;
        default:
            break;
        }
        break;
    case WalCancelState::doorOpenedAfterUnlockCancelPossible:
        switch (event.type)
        {
        case EventType::doorOpenStatus:
        case EventType::doorLockStatus:
            if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked() && _mainWalState != MainWalState::executing)
            {
                _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancelNotPossible;
            }
            break;
        case EventType::remoteControlUnlockButtonPressed:
        case EventType::remoteControlLockButtonPressed:
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
            if (DoorsBootAndBonnetAreClosed() && AllDoorsAreLocked() && _mainWalState != MainWalState::executing)
            {
                _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancelNotPossible;
            }
            break;
        case EventType::vehicleSpeed:
            if (_storedReportedStatuses.vehicleSpeed > 0)
            {
                _walCancelState = WalCancelState::noDoorOpenedAfterUnlockCancelNotPossible;
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
        _iCanWalProcessingOut->Trace("Changing _walCancelState from %s to %s", ToString(previousWalCancelState), ToString(_walCancelState));
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
        _iCanWalProcessingOut->Trace("Changing _doorOpenSequenceAfterUnlockState from %s to %s", ToString(previousDoorOpenSequenceAfterUnlockState), ToString(_doorOpenSequenceAfterUnlockState));
    }

    // The main WAL state machine waits for all conditions to become "go"
    // At that point it waits a certain period before actually locking the car
    // The execution of locking the car takes some time after which we can be certain that the
    // received statuses are "no go" and we can return to the initial state
    // At all times during "go" or "executing" the process can be stopped via the remote control
    // which is monitores by the WAL cancel state machine
    auto previousMainWalState = _mainWalState;
    switch (_mainWalState)
    {
    case MainWalState::noGo:
// if (event.type == EventType::timerExpiry)
// {
//     _mainWalState = MainWalState::go;
//     StartTimer(Timers::stopWalExecution, TimerPeriod::stopWalExecutionShort);
// }
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
// case Timers::stopWalExecution:
//     StartTimer(Timers::intermediateCountdownSoundInterval, TimerPeriod::intermediateCountdownSoundInterval);
//     _mainWalState = MainWalState::noGo;
//     break;
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
        default:
            // EventForGoCondition will check the event type for us
            if (EventForGoCondition(event.type, frontPassengerSeatStateChanged, walCancelStateChanged))
            {
                if (!GoConditionActive())
                {
                    if (TimerRunning(Timers::closeWindowsAndRoofCanMessageInterval))
                    {
                        StopTimer(Timers::closeWindowsAndRoofCanMessageInterval);
                    }
                    StopTimer(Timers::stopWalExecution);
                    _mainWalState = MainWalState::noGo;
                }
            }
            break;
        }
        break;
    }
    if (_mainWalState != previousMainWalState)
    {
        _iCanWalProcessingOut->Trace("Changing _mainWalState from %s to %s", ToString(previousMainWalState), ToString(_mainWalState));
    }
}

void CanWalProcessing::StartTimer(int timerId, TimerPeriod timerPeriod)
{
    Timer* timer = Timer::FindTimer(timerId);
    if (timer->_running)
    {
        _iCanWalProcessingOut->StopTimer(timerId);
    }
    else
    {
        timer->_running = true;
    }

    _iCanWalProcessingOut->Trace("Starting timer \"%s\" with a period of %d ms", timer->_name, timerPeriod);
    _iCanWalProcessingOut->StartTimer(timerId, (int)timerPeriod);
}

void CanWalProcessing::StopTimer(int timerId)
{
    Timer* timer = Timer::FindTimer(timerId);
    if (timer->_running)
    {
        _iCanWalProcessingOut->Trace("Stopping timer \"%s\"", timer->_name);
        _iCanWalProcessingOut->StopTimer(timerId);
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
    _iCanWalProcessingOut->SendCanMessage(0x26E, sizeof(data), data);
}

void CanWalProcessing::SendCloseWindowsAndRoofAndFoldMirrorsMessage()
{
    unsigned char data[8] = { 0x1B, 0x1B, 0x1B, 0x52, 0xFF, 0xFF, 0xFF, 0xFF };
    _iCanWalProcessingOut->SendCanMessage(0x26E, sizeof(data), data);
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
