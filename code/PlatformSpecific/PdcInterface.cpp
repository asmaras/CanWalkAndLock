#include "PdcInterface.h"
#include <driver/gpio.h>

namespace PlatformSpecific
{
    PdcInterface::PdcInterface()
    {
        gpio_config_t io_conf = {};
        io_conf.pin_bit_mask = 1ULL << _buzzerGpioPin;
        io_conf.mode = GPIO_MODE_OUTPUT;
        gpio_config(&io_conf);
        IPsPdcInterfaceEnableBuzzer();
    }

    void PdcInterface::IPsPdcInterfaceEnableBuzzer()
    {
        gpio_set_level(_buzzerGpioPin, 1);
    }

    void PdcInterface::IPsPdcInterfaceDisableBuzzer()
    {
        gpio_set_level(_buzzerGpioPin, 0);
    }
}