#pragma once

#include "IPdcInterface.h"
#include <driver/gpio.h>

namespace PlatformSpecific
{
    class PdcInterface : public PlatformSpecific::IPdcInterface::Input
    {
    public:
        PdcInterface(gpio_num_t pdcBuzzerPin);

        // PlatformSpecific::IPdcInterface::Input
        void IPsPdcInterfaceEnableBuzzer() override;
        void IPsPdcInterfaceDisableBuzzer() override;

    private:
        const gpio_num_t _pdcBuzzerPin;
    };
}