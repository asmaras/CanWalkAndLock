#pragma once

#include "IRemoteControl.h"
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <freertos/task.h>

namespace PlatformSpecific
{
    class RemoteControl : public PlatformSpecific::IRemoteControl::Input
    {
    public:
        RemoteControl(gpio_num_t remoteControlPowerPin, gpio_num_t remoteControlLockPin, SemaphoreHandle_t& processingMutex);

    private:
        // PlatformSpecific::IRemoteControl::Input
        void IPsRemoteControlLockSinglePress(PlatformSpecific::IRemoteControl::Output* iOutput) override;
        void IPsRemoteControlLockDoublePress(PlatformSpecific::IRemoteControl::Output* iOutput) override;

        static void RemoteControlTask(void* pvParameters);

        const gpio_num_t _remoteControlPowerPin;
        const gpio_num_t _remoteControlLockPin;
        enum class Timings
        {
            powerUp = 250,
            buttonPress = 250,
            buttonRelease = 500
        };
        class NotificationValues
        {
        public:
            static constexpr uint32_t singlePress = 0x01;
            static constexpr uint32_t doublePress = 0x02;
        };
        const SemaphoreHandle_t& _processingMutex;
        static constexpr const char* _logTag = "CanInterface";
        TaskHandle_t _remoteControlTaskHandle;
        PlatformSpecific::IRemoteControl::Output* _iOutput;
    };
}