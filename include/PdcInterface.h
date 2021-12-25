#pragma once

#include <driver/gpio.h>

class PdcInterface
{
public:
    PdcInterface();

    void EnableBuzzer();
    void DisableBuzzer();
    static void TestTask(void* pvParameters);

private:
    const gpio_num_t _buzzerGpioPin = GPIO_NUM_32;
};