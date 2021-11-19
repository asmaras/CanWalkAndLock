#pragma once

#include "ICanInterface.h"
#include "ICanWalProcessingAdapter.h"

class CanInterface : public ICanInterface
{
public:
    CanInterface();

    ICanWalProcessingAdapter* _iCanWalProcessingAdapter = nullptr;

    void Start();
    void SendMessage(int id, int length, const unsigned char* data) override;
    
private:
    static void CanReceiveTask(void* pvParameters);
};