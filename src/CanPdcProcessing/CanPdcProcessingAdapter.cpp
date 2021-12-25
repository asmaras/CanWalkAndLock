#include "CanPdcProcessing/CanPdcProcessingAdapter.h"

CanPdcProcessingAdapter::CanPdcProcessingAdapter()
{
    _canInterface.SetOutputInterface(this);
}

void CanPdcProcessingAdapter::SetOutputInterface(ICanPdcProcessing::Can::Input* iCanPdcProcessingCan)
{
    _iCanPdcProcessingCan = iCanPdcProcessingCan;
}

void CanPdcProcessingAdapter::Trace(const char* text, ...)
{
    va_list arguments;
    va_start(arguments, text);
    _osAbstraction.Trace("CanPdc", text, arguments);
    va_end(arguments);
}

void CanPdcProcessingAdapter::SendCanMessage(int id, int length, const unsigned char* data)
{
    _canInterface.SendCanMessage(id, length, data);
}

void CanPdcProcessingAdapter::HandleCanMessage(int id, int length, const unsigned char* data)
{
    _iCanPdcProcessingCan->HandleCanMessage(id, length, data);
}

void CanPdcProcessingAdapter::EnableBuzzer()
{
    _pdcInterface.EnableBuzzer();
}

void CanPdcProcessingAdapter::DisableBuzzer()
{
    _pdcInterface.DisableBuzzer();
}
