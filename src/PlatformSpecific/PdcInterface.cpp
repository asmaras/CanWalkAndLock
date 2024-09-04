#include "PdcInterface.h"
#include <driver/gpio.h>

namespace PlatformSpecific
{
    PdcInterface::PdcInterface(gpio_num_t pdcBuzzerPin) :
        _pdcBuzzerPin(pdcBuzzerPin)
    {
        gpio_config_t gpioConfig = {};
        gpioConfig.pin_bit_mask = 1ULL << _pdcBuzzerPin;
        gpioConfig.mode = GPIO_MODE_OUTPUT;
        gpio_config(&gpioConfig);
        IPsPdcInterfaceEnableBuzzer();
    }

    void PdcInterface::IPsPdcInterfaceEnableBuzzer()
    {
        gpio_set_level(_pdcBuzzerPin, 1);
    }

    void PdcInterface::IPsPdcInterfaceDisableBuzzer()
    {
        gpio_set_level(_pdcBuzzerPin, 0);
    }
}