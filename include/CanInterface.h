#pragma once

#include "ICanInterface.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <list>

class CanInterface
{
public:
    CanInterface();
    void SetOutputInterface(ICanInterface::Output* iOutput);
    static void Start();
    void SendCanMessage(int id, int length, const unsigned char* data);
    
private:
    class CommonHandling
    {
    public:
        CommonHandling(SemaphoreHandle_t& processingMutex);
        void Start();
        void SendCanMessage(int id, int length, const unsigned char* data);

        std::list<ICanInterface::Output*> _iOutputs;
        
    private:
        static void CanReceiveTask(void* pvParameters);
        
        const SemaphoreHandle_t& _processingMutex;
        const char* _logTag = "CanInterface";
    };
    static CommonHandling _commonHandling;
};