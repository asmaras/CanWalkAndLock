#include "CanWalProcessingAdapter.h"

CanWalProcessingAdapter::CanWalProcessingAdapter()
{
    _iPsOsAbstraction = nullptr;
    _iPiOs = nullptr;
    _iPsCanInterface = nullptr;
    _iPiCan = nullptr;
    _iPsRemoteControl = nullptr;
    _iPiRemoteControl = nullptr;
    _iPsSoundPlayer = nullptr;
}

void CanWalProcessingAdapter::SetOutputInterfaces(
    PlatformSpecific::IOsAbstraction::Input* iPsOsAbstraction,
    PlatformIndependent::Commons::IOs::Input* iPiOs,
    PlatformSpecific::ICanInterface::Input* iPsCanInterface,
    PlatformIndependent::Commons::ICan::Input* iPiCan,
    PlatformSpecific::IRemoteControl::Input* iPsRemoteControl,
    PlatformIndependent::Commons::IRemoteControl::Input* iPiRemoteControl,
    PlatformSpecific::ISoundPlayer::Input* iPsSoundPlayer
)
{
    _iPsOsAbstraction = iPsOsAbstraction;
    _iPiOs = iPiOs;
    _iPsCanInterface = iPsCanInterface;
    _iPiCan = iPiCan;
    _iPsRemoteControl = iPsRemoteControl;
    _iPiRemoteControl = iPiRemoteControl;
    _iPsSoundPlayer = iPsSoundPlayer;
}

void CanWalProcessingAdapter::IpsOsAbstractionHandleExpiredTimer(int timerId)
{
    // Cleanup first before calling the function to handle the timer expiry
    // If the handling function starts the timer again we will not inadvertedly clean it up
    _platformSpecificTimerHandleByTimerId.erase(timerId);
    _iPiOs->IPiOsHandleExpiredTimer(timerId);
}

void CanWalProcessingAdapter::IPiOsStartTimer(int timerId, int timerPeriodInMilliSeconds)
{
    auto _platformSpecificTimerHandle = _iPsOsAbstraction->IpsOsAbstractionStartTimer(timerId, timerPeriodInMilliSeconds, this);
    _platformSpecificTimerHandleByTimerId[timerId] = _platformSpecificTimerHandle;
}

void CanWalProcessingAdapter::IPiOsStopTimer(int timerId)
{
    _iPsOsAbstraction->IpsOsAbstractionStopTimer(_platformSpecificTimerHandleByTimerId[timerId]);
    _platformSpecificTimerHandleByTimerId.erase(timerId);
}

void CanWalProcessingAdapter::IPiOsTrace(const char* text, va_list arguments)
{
    _iPsOsAbstraction->IpsOsAbstractionTrace("CanWal", text, arguments);
}

void CanWalProcessingAdapter::IPsCanInterfaceHandleCanMessage(int id, int length, const unsigned char* data)
{
    _iPiCan->IPiCanHandleCanMessage((PlatformIndependent::Commons::ICan::CanId)id, length, data);
}

void CanWalProcessingAdapter::IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data)
{
    _iPsCanInterface->IpsCanInterfaceSendCanMessage((int)id, length, data);
}

void CanWalProcessingAdapter::IPsRemoteControlOperationFinished()
{
    _iPiRemoteControl->IPiRemoteControlOperationFinished();
}

void CanWalProcessingAdapter::IPiRemoteControlLockSinglePress()
{
    _iPsRemoteControl->IPsRemoteControlLockSinglePress(this);
}

void CanWalProcessingAdapter::IPiRemoteControlLockDoublePress()
{
    _iPsRemoteControl->IPsRemoteControlLockDoublePress(this);
}

void CanWalProcessingAdapter::IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound sound)
{
    _iPsSoundPlayer->IpsSoundPlayerPlaySound((PlatformSpecific::ISoundPlayer::Input::Sound)sound);
}

void CanWalProcessingAdapter::IPiSoundStopPlayingSound()
{
    _iPsSoundPlayer->IpsSoundPlayerStopPlayingSound();
}
