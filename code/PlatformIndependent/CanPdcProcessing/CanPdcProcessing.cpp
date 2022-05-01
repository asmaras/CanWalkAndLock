#include "CanPdcProcessing.h"

namespace PlatformIndependent
{
    CanPdcProcessing::CanPdcProcessing()
    {
        _storedReportedStatuses.vehicleSpeed = -1;
        _storedReportedStatuses.automaticGearShifterPosition = ' ';
        _buzzerState = BuzzerState::enabled;

        _iPdc = nullptr;
    }

    void CanPdcProcessing::SetOutputInterfaces(
        PlatformIndependent::Commons::IOs::Output* iOs,
        PlatformIndependent::Commons::ICan::Output* iCan,
        PlatformIndependent::Commons::IPdc::Output* iPdc
    )
    {
        PlatformIndependent::Commons::Os::SetOutputInterface(iOs);
        _iPdc = iPdc;
    }

    void CanPdcProcessing::HandleExpiredTimer(int timerId, const char* timerName)
    {
        Event event;
        event.type = EventType::timerExpiry;
        event.timerExpiry.timerId = timerId;
        event.timerExpiry.timerName = timerName;
        HandleEvent(event);
    }

    void CanPdcProcessing::IPiCanHandleCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data)
    {
        switch (id)
        {
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
        case PlatformIndependent::Commons::ICan::CanId::gearShifterPosition:
        {
            // The automatic gear shifter position is encoded in bits 0-3 of byte 0
            // Bits 0-3 respectively represent P-R-N-D
            // Bits 4-7 are an inverted version of bits 0-3
            bool validPosition = true;
            char position;
            if ((data[0] & 0x01) != 0)
            {
                position = 'P';
            }
            else if ((data[0] & 0x02) != 0)
            {
                position = 'R';
            }
            else if ((data[0] & 0x04) != 0)
            {
                position = 'N';
            }
            else if ((data[0] & 0x08) != 0)
            {
                position = 'D';
            }
            else
            {
                validPosition = false;
            }
            if (validPosition && position != _storedReportedStatuses.automaticGearShifterPosition)
            {
                _storedReportedStatuses.automaticGearShifterPosition = position;
                Event event;
                event.type = EventType::automaticGearShifterPosition;
                HandleEvent(event);
            }
        }
        break;
        default:
            break;
        }
    }

    const char* CanPdcProcessing::ToString(BuzzerState buzzerState)
    {
        switch (buzzerState)
        {
        case BuzzerState::enabled: return "enabled";
        case BuzzerState::disabled: return "disabled";
        default: return "";
        }
    }

    void CanPdcProcessing::HandleEvent(Event event)
    {
        TraceEvent(event);

        // The rules are:
        // 1 - if shifter in P we want the buzzer to be off
        // 2 - if shifter in R we want the buzzer to be on
        // If rules 1 and 2 don't apply:
        // 3 - if speed goes to 0 the buzzer must be switched off after a while if no other event occurs
        // 4 - if speed is above a certain limit the buzzer must be off
        // 5 - if speed is under the limit the buzzer must be on
        // 6 - if shifter is changed to N the buzzer must be switched according to the speed
        // 7 - if shifter is changed to D and the speed is 0 the buzzer must be switched on and remain on for a wile if no other event occurs
        auto previousBuzzerState = _buzzerState;
        switch (_buzzerState)
        {
        case BuzzerState::enabled:
            switch (event.type)
            {
            case EventType::vehicleSpeed:
                // Rule 2 - if shifter in R we want the buzzer to be on
                // Implemented by not handling vehicleSpeed events
                if (_storedReportedStatuses.automaticGearShifterPosition == 'R')
                {
                    Trace("Not handling event because automaticGearShifterPosition=%c", _storedReportedStatuses.automaticGearShifterPosition);
                }
                else
                {
                    // Rule 3 - if speed goes to 0 the buzzer must be switched off after a while if no other event occurs
                    // Implemented by (re)starting timer
                    if (_storedReportedStatuses.vehicleSpeed == 0)
                    {
                        StartTimer(Timers::buzzerOffWait, TimerPeriod::buzzerOffWait);
                    }
                    else
                    {
                        // We don't need the timer to be running (rule 3 doesn't apply) so stop it in case it's running
                        if (TimerRunning(Timers::buzzerOffWait)) StopTimer(Timers::buzzerOffWait);
                        // Rule 4 - if speed is above a certain limit the buzzer must be off
                        if (_storedReportedStatuses.vehicleSpeed >= _switchOffSpeed)
                        {
                            _iPdc->IPiPdcDisableBuzzer();
                            _buzzerState = BuzzerState::disabled;
                        }
                    }
                }
                break;
            case EventType::automaticGearShifterPosition:
                switch (_storedReportedStatuses.automaticGearShifterPosition)
                {
                case 'P':
                    // Rule 1 - if shifter in P we want the buzzer to be off
                    _iPdc->IPiPdcDisableBuzzer();
                    // We don't need the timer to be running (rule 3 doesn't apply) so stop it in case it's running
                    if (TimerRunning(Timers::buzzerOffWait)) StopTimer(Timers::buzzerOffWait);
                    _buzzerState = BuzzerState::disabled;
                    break;
                case 'R':
                    // Rule 2 - if shifter in R we want the buzzer to be on
                    // The buzzer is already on so nothing has to be done
                    // We don't need the timer to be running (rule 3 doesn't apply) so stop it in case it's running
                    if (TimerRunning(Timers::buzzerOffWait)) StopTimer(Timers::buzzerOffWait);
                    break;
                case 'N':
                    // Rule 6 - if shifter is changed to N the buzzer must be switched according to the speed
                    // The buzzer is currently on so check the speed to see if it should be switched off
                    if (_storedReportedStatuses.vehicleSpeed == 0 || _storedReportedStatuses.vehicleSpeed >= _switchOffSpeed)
                    {
                        _iPdc->IPiPdcDisableBuzzer();
                        // We don't need the timer to be running (rule 3 doesn't apply) so stop it in case it's running
                        if (TimerRunning(Timers::buzzerOffWait)) StopTimer(Timers::buzzerOffWait);
                        _buzzerState = BuzzerState::disabled;
                    }
                    break;
                case 'D':
                    // Rule 7 - if shifter is changed to D and the speed is 0 the buzzer must be switched on and remain on for a wile if no other event occurs
                    // Implemented by (re)starting timer
                    if (_storedReportedStatuses.vehicleSpeed == 0)
                    {
                        StartTimer(Timers::buzzerOffWait, TimerPeriod::buzzerOffWait);
                    }
                    break;
                }
                break;
            case EventType::timerExpiry:
                switch (event.timerExpiry.timerId)
                {
                case Timers::buzzerOffWait:
                    _iPdc->IPiPdcDisableBuzzer();
                    _buzzerState = BuzzerState::disabled;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        case BuzzerState::disabled:
            switch (event.type)
            {
            case EventType::vehicleSpeed:
                // Rule 1 - if shifter in P we want the buzzer to be off
                // Implemented by not handling vehicleSpeed events
                if (_storedReportedStatuses.automaticGearShifterPosition == 'P')
                {
                    Trace("Not handling event because automaticGearShifterPosition=%c", _storedReportedStatuses.automaticGearShifterPosition);
                }
                else
                {
                    // Rule 5 - if speed is under the limit the buzzer must be on
                    if (_storedReportedStatuses.vehicleSpeed > 0 && _storedReportedStatuses.vehicleSpeed < _switchOffSpeed)
                    {
                        _iPdc->IPiPdcEnableBuzzer();
                        _buzzerState = BuzzerState::enabled;
                    }
                }
                break;
            case EventType::automaticGearShifterPosition:
                switch (_storedReportedStatuses.automaticGearShifterPosition)
                {
                case 'R':
                    // Rule 2 - if shifter in R we want the buzzer to be on
                    _iPdc->IPiPdcEnableBuzzer();
                    _buzzerState = BuzzerState::enabled;
                    break;
                case 'N':
                    // Rule 6 - if shifter is changed to N the buzzer must be switched according to the speed
                    // The buzzer is currently off so check the speed to see if it should be switched on
                    // Note that in reality the previous position will have been 'D' (because 'R' would have changed the state) and the
                    // buzzer will already be switched according to the speed
                    // To be complete, this is implemented
                    if (_storedReportedStatuses.vehicleSpeed > 0 && _storedReportedStatuses.vehicleSpeed < _switchOffSpeed)
                    {
                        _iPdc->IPiPdcEnableBuzzer();
                        _buzzerState = BuzzerState::enabled;
                    }
                    break;
                case 'D':
                    // Rule 7 - if shifter is changed to D and the speed is 0 the buzzer must be switched on and remain on for a wile if no other event occurs
                    // Implemented by switching the buzzer on and starting timer
                    if (_storedReportedStatuses.vehicleSpeed == 0)
                    {
                        _iPdc->IPiPdcEnableBuzzer();
                        StartTimer(Timers::buzzerOffWait, TimerPeriod::buzzerOffWait);
                        _buzzerState = BuzzerState::enabled;
                    }
                    break;
                }
                break;
            default:
                break;
            }
            break;
        }
        if (_buzzerState != previousBuzzerState)
        {
            Trace("Changing _buzzerState from %s to %s", ToString(previousBuzzerState), ToString(_buzzerState));
        }
    }

    void CanPdcProcessing::TraceEvent(Event event)
    {
        switch (event.type)
        {
        case EventType::timerExpiry:
            Trace("Event timerExpiry(%s)", event.timerExpiry.timerName);
            break;
        case EventType::vehicleSpeed:
            Trace("Event vehicleSpeed(%d)", _storedReportedStatuses.vehicleSpeed);
            break;
        case EventType::automaticGearShifterPosition:
            Trace("Event automaticGearShifterPosition(%c)", _storedReportedStatuses.automaticGearShifterPosition);
            break;
        default:
            break;
        }
    }

    bool CanPdcProcessing::CheckShifterChangedToDrivingPosition(char position)
    {
        // Check if the shifter position is changed from a non-driving to a driving position
        bool result =
            (
                _storedReportedStatuses.automaticGearShifterPosition != 'D' &&
                position == 'D'
                )
            ||
            (
                _storedReportedStatuses.automaticGearShifterPosition != 'R' &&
                position == 'R'
                );
        return result;
    }

    bool CanPdcProcessing::CheckShifterChangedToParkingPosition(char position)
    {
        // Check if the shifter position is changed to the parking position
        bool result =
            _storedReportedStatuses.automaticGearShifterPosition != 'P' &&
            position == 'P';
        return result;
    }

    void CanPdcProcessing::StartTimer(int timerId, TimerPeriod timerPeriod)
    {
        PlatformIndependent::Commons::Os::StartTimer(timerId, (int)timerPeriod);
    }
}