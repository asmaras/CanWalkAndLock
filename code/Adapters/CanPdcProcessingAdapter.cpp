#include "CanPdcProcessingAdapter.h"

CanPdcProcessingAdapter::CanPdcProcessingAdapter()
{
    _iPsOsAbstraction = nullptr;
    _iPiOs = nullptr;
    _iPsCanInterface = nullptr;
    _iPiCan = nullptr;
    _iPsPdcInterface = nullptr;
}

void CanPdcProcessingAdapter::SetOutputInterfaces(
    PlatformSpecific::IOsAbstraction::Input* iPsOsAbstraction,
    PlatformIndependent::Commons::IOs::Input* iPiOs,
    PlatformSpecific::ICanInterface::Input* iPsCanInterface,
    PlatformIndependent::Commons::ICan::Input* iPiCan,
    PlatformSpecific::IPdcInterface::Input* iPsPdcInterface
)
{
    _iPsOsAbstraction = iPsOsAbstraction;
    _iPiOs = iPiOs;
    _iPsCanInterface = iPsCanInterface;
    _iPiCan = iPiCan;
    _iPsPdcInterface = iPsPdcInterface;
}

void CanPdcProcessingAdapter::IpsOsAbstractionHandleExpiredTimer(int timerId)
{
    // Cleanup first before calling the function to handle the timer expiry
    // If the handling function starts the timer again we will not inadvertedly clean it up
    _platformSpecificTimerHandleByTimerId.erase(timerId);
    _iPiOs->IPiOsHandleExpiredTimer(timerId);
}

void CanPdcProcessingAdapter::IPiOsStartTimer(int timerId, int timerPeriodInMilliSeconds)
{
    auto _platformSpecificTimerHandle = _iPsOsAbstraction->IpsOsAbstractionStartTimer(timerId, timerPeriodInMilliSeconds, this);
    _platformSpecificTimerHandleByTimerId[timerId] = _platformSpecificTimerHandle;
}

void CanPdcProcessingAdapter::IPiOsStopTimer(int timerId)
{
    _iPsOsAbstraction->IpsOsAbstractionStopTimer(_platformSpecificTimerHandleByTimerId[timerId]);
    _platformSpecificTimerHandleByTimerId.erase(timerId);
}

void CanPdcProcessingAdapter::IPiOsTrace(const char* text, va_list arguments)
{
    _iPsOsAbstraction->IpsOsAbstractionTrace("CanPdc", text, arguments);
}

void CanPdcProcessingAdapter::IPsCanInterfaceHandleCanMessage(int id, int length, const unsigned char* data)
{
    _iPiCan->IPiCanHandleCanMessage((PlatformIndependent::Commons::ICan::CanId)id, length, data);
}

void CanPdcProcessingAdapter::IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data)
{
    _iPsCanInterface->IpsCanInterfaceSendCanMessage((int)id, length, data);
}

void CanPdcProcessingAdapter::IPiPdcEnableBuzzer()
{
    _iPsPdcInterface->IPsPdcInterfaceEnableBuzzer();
}

void CanPdcProcessingAdapter::IPiPdcDisableBuzzer()
{
    _iPsPdcInterface->IPsPdcInterfaceDisableBuzzer();
}
