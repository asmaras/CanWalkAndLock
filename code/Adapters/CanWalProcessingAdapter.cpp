#include "CanWalProcessingAdapter.h"

CanWalProcessingAdapter::CanWalProcessingAdapter()
{
    _iPsNvs = nullptr;
    _iPsOsAbstraction = nullptr;
    _iPiOs = nullptr;
    _iPsCanInterface = nullptr;
    _iPiCan = nullptr;
    _iPsRemoteControl = nullptr;
    _iPiRemoteControl = nullptr;
    _iPsSoundPlayer = nullptr;
    _nvsHandle = {};
    _platformSpecificTimerHandleByTimerId = {};
}

void CanWalProcessingAdapter::SetOutputInterfaces(
    PlatformSpecific::INvs::Input* iPsNvs,
    PlatformSpecific::IOsAbstraction::Input* iPsOsAbstraction,
    PlatformIndependent::Commons::IOs::Input* iPiOs,
    PlatformSpecific::ICanInterface::Input* iPsCanInterface,
    PlatformIndependent::Commons::ICan::Input* iPiCan,
    PlatformSpecific::IRemoteControl::Input* iPsRemoteControl,
    PlatformIndependent::Commons::IRemoteControl::Input* iPiRemoteControl,
    PlatformSpecific::ISoundPlayer::Input* iPsSoundPlayer
)
{
    _iPsNvs = iPsNvs;
    _iPsOsAbstraction = iPsOsAbstraction;
    _iPiOs = iPiOs;
    _iPsCanInterface = iPsCanInterface;
    _iPiCan = iPiCan;
    _iPsRemoteControl = iPsRemoteControl;
    _iPiRemoteControl = iPiRemoteControl;
    _iPsSoundPlayer = iPsSoundPlayer;
}

void CanWalProcessingAdapter::Start()
{
    constexpr char nvsNamespaceName[PlatformSpecific::INvs::Input::namespaceNameLength] = { "CanWal" };
    _nvsHandle = _iPsNvs->IPsNvsOpen(nvsNamespaceName);
}

bool CanWalProcessingAdapter::IPiStoreGetEnableWAL()
{
    constexpr char nvsKey[PlatformSpecific::INvs::Input::keyLength] = { "EnableWal" };
    return _iPsNvs->IPsNvsGetBool(_nvsHandle, nvsKey, true);
}

void CanWalProcessingAdapter::IPiStoreSetEnableWAL(bool enable)
{
    constexpr char nvsKey[PlatformSpecific::INvs::Input::keyLength] = { "EnableWal" };
    _iPsNvs->IPsNvsSetBool(_nvsHandle, nvsKey, enable);
}

bool CanWalProcessingAdapter::IPiStoreGetMayCloseWindowsAndRoof()
{
    constexpr char nvsKey[PlatformSpecific::INvs::Input::keyLength] = { "MayCloseWandR" };
    return _iPsNvs->IPsNvsGetBool(_nvsHandle, nvsKey, true);
}

void CanWalProcessingAdapter::IPiStoreSetMayCloseWindowsAndRoof(bool enable)
{
    constexpr char nvsKey[PlatformSpecific::INvs::Input::keyLength] = { "MayCloseWandR" };
    _iPsNvs->IPsNvsSetBool(_nvsHandle, nvsKey, enable);
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
