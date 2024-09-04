#pragma once

#include "PlatformSpecific/IOsAbstraction.h"
#include "PlatformIndependent/Commons/IOs.h"
#include "PlatformSpecific/ICanInterface.h"
#include "PlatformIndependent/Commons/ICan.h"
#include "PlatformSpecific/IPdcInterface.h"
#include "PlatformIndependent/Commons/IPdc.h"
#include <map>

class CanPdcProcessingAdapter :
    public PlatformSpecific::IOsAbstraction::Output,
    public PlatformIndependent::Commons::IOs::Output,
    public PlatformSpecific::ICanInterface::Output,
    public PlatformIndependent::Commons::ICan::Output,
    public PlatformIndependent::Commons::IPdc::Output
{
public:
    CanPdcProcessingAdapter();
    void SetOutputInterfaces(
        PlatformSpecific::IOsAbstraction::Input* iPsOsAbstraction,
        PlatformIndependent::Commons::IOs::Input* iPiOs,
        PlatformSpecific::ICanInterface::Input* iPsCanInterface,
        PlatformIndependent::Commons::ICan::Input* iPiCan,
        PlatformSpecific::IPdcInterface::Input* iPsPdcInterface
    );

private:
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
    // Platform independent PDC functions -> adapter
    // PlatformIndependent::Commons::IPdc::Output
    void IPiPdcEnableBuzzer() override;
    void IPiPdcDisableBuzzer() override;

    // Adapter -> Platform specific OS functions
    PlatformSpecific::IOsAbstraction::Input* _iPsOsAbstraction;
    // Adapter -> Platform independent OS functions
    PlatformIndependent::Commons::IOs::Input* _iPiOs;
    // Adapter -> Platform specific CAN functions
    PlatformSpecific::ICanInterface::Input* _iPsCanInterface;
    // Adapter -> Platform independent CAN functions
    PlatformIndependent::Commons::ICan::Input* _iPiCan;
    // Adapter -> Platform specific PDC functions
    PlatformSpecific::IPdcInterface::Input* _iPsPdcInterface;

    std::map<int, PlatformSpecific::IOsAbstraction::Input::TimerHandle> _platformSpecificTimerHandleByTimerId;
};