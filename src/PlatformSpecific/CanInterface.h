#pragma once

#include "ICanInterface.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <list>

namespace PlatformSpecific
{
    class CanInterface : public PlatformSpecific::ICanInterface::Input
    {
    public:
        CanInterface(gpio_num_t canTxPin, gpio_num_t canRxPin, SemaphoreHandle_t& processingMutex);
        void AddOutputInterface(PlatformSpecific::ICanInterface::Output* iOutput);
        void Start();

    private:
        // PlatformSpecific::ICanInterface::Input
        void IpsCanInterfaceSendCanMessage(int id, int length, const unsigned char* data) override;

        static void CanReceiveTask(void* pvParameters);

        const gpio_num_t _canTxPin;
        const gpio_num_t _canRxPin;
        const SemaphoreHandle_t& _processingMutex;
        static constexpr const char* _logTag = "CanInterface";
        std::list<PlatformSpecific::ICanInterface::Output*> _iOutputs;
    };
}