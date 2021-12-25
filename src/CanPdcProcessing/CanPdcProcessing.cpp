#include "CanPdcProcessing/CanPdcProcessing.h"

CanPdcProcessing::CanPdcProcessing()
{
    _storedReportedStatuses.vehicleSpeed = -1;
    _buzzerState = BuzzerState::enabled;
}

void CanPdcProcessing::SetOutputInterfaces(ICanPdcProcessing::Os::Output* iOs, ICanPdcProcessing::Pdc::Output* iPdc)
{
    _iOs = iOs;
    _iPdc = iPdc;
}

void CanPdcProcessing::HandleCanMessage(int id, int length, const unsigned char* data)
{
    switch ((CanId)id)
    {
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

    auto previousBuzzerState = _buzzerState;
    switch (_buzzerState)
    {
    case BuzzerState::enabled:
        switch (event.type)
        {
        case EventType::vehicleSpeed:
            if (_storedReportedStatuses.vehicleSpeed == 0 || _storedReportedStatuses.vehicleSpeed >= 200)
            {
                _iPdc->DisableBuzzer();
                _buzzerState = BuzzerState::disabled;
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
            if (_storedReportedStatuses.vehicleSpeed > 0 && _storedReportedStatuses.vehicleSpeed < 200)
            {
                _iPdc->EnableBuzzer();
                _buzzerState = BuzzerState::enabled;
            }
            break;
        default:
            break;
        }
        break;
    }
    if (_buzzerState != previousBuzzerState)
    {
        _iOs->Trace("Changing _buzzerState from %s to %s", ToString(previousBuzzerState), ToString(_buzzerState));
    }
}

void CanPdcProcessing::TraceEvent(Event event)
{
    switch (event.type)
    {
    case EventType::vehicleSpeed:
        _iOs->Trace("Event vehicleSpeed(%d)",
            _storedReportedStatuses.vehicleSpeed
        );
        break;
    default:
        break;
    }
}
