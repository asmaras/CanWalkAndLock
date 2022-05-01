#pragma once

#include "IPdcInterface.h"
#include <driver/gpio.h>

namespace PlatformSpecific
{
    class PdcInterface : public PlatformSpecific::IPdcInterface::Input
    {
    public:
        PdcInterface();

        // PlatformSpecific::IPdcInterface::Input
        void IPsPdcInterfaceEnableBuzzer() override;
        void IPsPdcInterfaceDisableBuzzer() override;

    private:
        static const gpio_num_t _buzzerGpioPin = GPIO_NUM_32;
    };
}