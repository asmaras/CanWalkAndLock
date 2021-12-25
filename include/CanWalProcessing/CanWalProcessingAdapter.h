#pragma once

#include "ICanWalProcessing.h"
#include "OsAbstraction.h"
#include "CanInterface.h"

class CanWalProcessingAdapter :
    public ICanWalProcessing::Os::Output,
    public IOsAbstraction::Output,
    public ICanWalProcessing::Can::Output,
    public ICanInterface::Output
{
public:
    CanWalProcessingAdapter();
    void SetOutputInterfaces(ICanWalProcessing::Os::Input* iCanWalProcessingOs, ICanWalProcessing::Can::Input* iCanWalProcessingCan);

private:
    // ICanWalProcessing::Os::Output
    void StartTimer(int timerId, int timerPeriodInMilliSeconds) override;
    void StopTimer(int timerId) override;
    void Trace(const char* text, ...) override;
    // IOsAbstraction::Output
    void HandleExpiredTimer(int timerId) override;
    // ICanWalProcessing::Can::Output
    void SendCanMessage(int id, int length, const unsigned char* data) override;
    // ICanInterface::Output
    void HandleCanMessage(int id, int length, const unsigned char* data) override;

    ICanWalProcessing::Os::Input* _iCanWalProcessingOs = nullptr;
    ICanWalProcessing::Can::Input* _iCanWalProcessingCan = nullptr;
    OsAbstraction _osAbstraction;
    CanInterface _canInterface;
};