#include "CanWalProcessing/CanWalProcessingAdapter.h"

CanWalProcessingAdapter::CanWalProcessingAdapter()
{
    _osAbstraction.SetOutputInterface(this);
    _canInterface.SetOutputInterface(this);
}

void CanWalProcessingAdapter::SetOutputInterfaces(ICanWalProcessing::Os::Input* iCanWalProcessingOs, ICanWalProcessing::Can::Input* iCanWalProcessingCan)
{
    _iCanWalProcessingOs = iCanWalProcessingOs;
    _iCanWalProcessingCan = iCanWalProcessingCan;
}

void CanWalProcessingAdapter::StartTimer(int timerId, int timerPeriodInMilliSeconds)
{
    _osAbstraction.StartTimer(timerId, timerPeriodInMilliSeconds);
}

void CanWalProcessingAdapter::StopTimer(int timerId)
{
    _osAbstraction.StopTimer(timerId);
}

void CanWalProcessingAdapter::Trace(const char* text, ...)
{
    va_list arguments;
    va_start(arguments, text);
    _osAbstraction.Trace("CanWal", text, arguments);
    va_end(arguments);
}

void CanWalProcessingAdapter::HandleExpiredTimer(int timerId)
{
    _iCanWalProcessingOs->HandleExpiredTimer(timerId);
}

void CanWalProcessingAdapter::SendCanMessage(int id, int length, const unsigned char* data)
{
    _canInterface.SendCanMessage(id, length, data);
}

void CanWalProcessingAdapter::HandleCanMessage(int id, int length, const unsigned char* data)
{
    _iCanWalProcessingCan->HandleCanMessage(id, length, data);
}
