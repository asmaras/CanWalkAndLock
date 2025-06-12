#pragma once

#include "ICanInterface.h"
#include "INvs.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <list>
#include <stdint.h>

namespace PlatformSpecific
{
    class CanInterface : public PlatformSpecific::ICanInterface::Input
    {
    public:
        CanInterface(gpio_num_t canTxPin, gpio_num_t canRxPin, SemaphoreHandle_t& processingMutex);
        void AddOutputInterface(PlatformSpecific::ICanInterface::Output* iOutput);
        void SetOutputInterfaceToNvs(PlatformSpecific::INvs::Input* iPsNvs);
        void Start();
        void StartCanDriver();
        void StopCanDriver();
        void StartCanReceiveTask();

        uint16_t _canSendErrors = 0;
        uint16_t _canReceiveErrors = 0;

    private:
        // PlatformSpecific::ICanInterface::Input
        void IpsCanInterfaceSendCanMessage(int id, int length, const unsigned char* data) override;

        static void CanReceiveTask(void* pvParameters);
        void CanReceiveTask();

        const gpio_num_t _canTxPin;
        const gpio_num_t _canRxPin;
        const SemaphoreHandle_t& _processingMutex;
        static constexpr const char* _logTag = "CanInterface";
        std::list<PlatformSpecific::ICanInterface::Output*> _iOutputs = {};
        PlatformSpecific::INvs::Input* _iPsNvs = {};
        PlatformSpecific::INvs::Input::NvsHandle _nvsHandle = {};
        TaskHandle_t _canReceiveTaskHandle;
    };
}