#pragma once

#include "PlatformSpecific/INvs.h"
#include "PlatformIndependent/Commons/IStore.h"
#include "PlatformSpecific/IOsAbstraction.h"
#include "PlatformIndependent/Commons/IOs.h"
#include "PlatformSpecific/ICanInterface.h"
#include "PlatformIndependent/Commons/ICan.h"
#include "PlatformSpecific/IRemoteControl.h"
#include "PlatformIndependent/Commons/IRemoteControl.h"
#include "PlatformSpecific/ISoundPlayer.h"
#include "PlatformIndependent/Commons/ISound.h"
#include <map>

class CanWalProcessingAdapter :
    public PlatformIndependent::Commons::IStore::Output,
    public PlatformSpecific::IOsAbstraction::Output,
    public PlatformIndependent::Commons::IOs::Output,
    public PlatformSpecific::ICanInterface::Output,
    public PlatformIndependent::Commons::ICan::Output,
    public PlatformSpecific::IRemoteControl::Output,
    public PlatformIndependent::Commons::IRemoteControl::Output,
    public PlatformIndependent::Commons::ISound::Output
{
public:
    CanWalProcessingAdapter();
    void SetOutputInterfaces(
        PlatformSpecific::INvs::Input* iPsNvs,
        PlatformSpecific::IOsAbstraction::Input* iPsOsAbstraction,
        PlatformIndependent::Commons::IOs::Input* iPiOs,
        PlatformSpecific::ICanInterface::Input* iPsCanInterface,
        PlatformIndependent::Commons::ICan::Input* iPiCan,
        PlatformSpecific::IRemoteControl::Input* iPsRemoteControl,
        PlatformIndependent::Commons::IRemoteControl::Input* iPiRemoteControl,
        PlatformSpecific::ISoundPlayer::Input* iPsSoundPlayer
    );
    void Start();

private:
    // Platform independent store functions -> adapter
    // PlatformIndependent::Commons::IStore
    bool IPiStoreGetEnableWAL() override;
    void IPiStoreSetEnableWAL(bool enable) override;
    bool IPiStoreGetMayCloseWindowsAndRoof() override;
    void IPiStoreSetMayCloseWindowsAndRoof(bool enable) override;
    // Platform specific OS functions -> adapter
    // PlatformSpecific::IOsAbstraction::Output
    void IpsOsAbstractionHandleExpiredTimer(int timerId) override;
    // Platform independent OS functions -> adapter
    // PlatformIndependent::Commons::IOs::Output
    void IPiOsStartTimer(int timerId, int timerPeriodInMilliSeconds) override;
    void IPiOsStopTimer(int timerId) override;
    void IPiOsTrace(const char* text, va_list arguments) override;
    // Platform specific CAN functions -> adapter
    // PlatformSpecific::ICanInterface::Output
    void IPsCanInterfaceHandleCanMessage(int id, int length, const unsigned char* data) override;
    // Platform independent CAN functions -> adapter
    // PlatformIndependent::Commons::ICan::Output
    void IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data) override;
    // Platform specific remote control functions -> adapter
    // PlatformSpecific::IRemoteControl::Output
    void IPsRemoteControlOperationFinished() override;
    // Platform independent remote control functions -> adapter
    // PlatformIndependent::Commons::IRemoteControl::Output
    void IPiRemoteControlLockSinglePress() override;
    void IPiRemoteControlLockDoublePress() override;
    // Platform independent sound functions -> adapter
    // PlatformIndependent::Commons::ISound::Output
    void IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound sound) override;
    void IPiSoundStopPlayingSound() override;

    // Adapter -> Platform specific NVS functions
    PlatformSpecific::INvs::Input* _iPsNvs;
    // Adapter -> Platform specific OS functions
    PlatformSpecific::IOsAbstraction::Input* _iPsOsAbstraction;
    // Adapter -> Platform independent OS functions
    PlatformIndependent::Commons::IOs::Input* _iPiOs;
    // Adapter -> Platform specific CAN functions
    PlatformSpecific::ICanInterface::Input* _iPsCanInterface;
    // Adapter -> Platform independent CAN functions
    PlatformIndependent::Commons::ICan::Input* _iPiCan;
    // Adapter -> Platform specific remote control functions
    PlatformSpecific::IRemoteControl::Input* _iPsRemoteControl;
    // Adapter -> Platform independent remote control functions
    PlatformIndependent::Commons::IRemoteControl::Input* _iPiRemoteControl;
    // Adapter -> Platform specific sound functions
    PlatformSpecific::ISoundPlayer::Input* _iPsSoundPlayer;

    PlatformSpecific::INvs::Input::NvsHandle _nvsHandle;
    std::map<int, PlatformSpecific::IOsAbstraction::Input::TimerHandle> _platformSpecificTimerHandleByTimerId;
};