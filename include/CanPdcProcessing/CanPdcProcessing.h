#pragma once

#include "ICanPdcProcessing.h"

class CanPdcProcessing : public ICanPdcProcessing::Can::Input
{
public:
    CanPdcProcessing();
    void SetOutputInterfaces(ICanPdcProcessing::Os::Output* iOs, ICanPdcProcessing::Pdc::Output* iPdc);

    // Function calls should always be mutually excluded
    // ICanPdcProcessing::Can::Input
    void HandleCanMessage(int id, int length, const unsigned char* data) override;

private:
    ICanPdcProcessing::Os::Output* _iOs = nullptr;
    ICanPdcProcessing::Pdc::Output* _iPdc = nullptr;
    // CAN IDs
    enum class CanId
    {
        vehicleSpeed = 0x1B4,
    };
    // Storage for received statuses
    struct
    {
        int vehicleSpeed;
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
        vehicleSpeed
    };
    // Event struct holding event type and parameters
    // Stored statuses are not included as parameters to make code less complex because it only has
    // to look at the stored statuses
    struct Event
    {
        EventType type;
    };
    void HandleEvent(Event event);
    void TraceEvent(Event event);
};
