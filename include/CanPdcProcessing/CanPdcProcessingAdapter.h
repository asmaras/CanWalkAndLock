#pragma once

#include "ICanPdcProcessing.h"
#include "OsAbstraction.h"
#include "CanInterface.h"
#include "PdcInterface.h"

class CanPdcProcessingAdapter :
    public ICanPdcProcessing::Os::Output,
    public ICanPdcProcessing::Can::Output,
    public ICanInterface::Output,
    public ICanPdcProcessing::Pdc::Output
{
public:
    CanPdcProcessingAdapter();
    void SetOutputInterface(ICanPdcProcessing::Can::Input* iCanPdcProcessingCan);

private:
    // ICanPdcProcessing::Os::Output
    void Trace(const char* text, ...) override;
    // ICanPdcProcessing::Can::Output
    void SendCanMessage(int id, int length, const unsigned char* data) override;
    // ICanInterface::Output
    void HandleCanMessage(int id, int length, const unsigned char* data) override;
    // ICanPdcProcessing::Pdc::Output
    void EnableBuzzer() override;
    void DisableBuzzer() override;

    ICanPdcProcessing::Can::Input* _iCanPdcProcessingCan = nullptr;
    OsAbstraction _osAbstraction;
    CanInterface _canInterface;
    PdcInterface _pdcInterface;
};