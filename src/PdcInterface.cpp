#include "PdcInterface.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

PdcInterface::PdcInterface()
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 1ULL << _buzzerGpioPin;
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);
    EnableBuzzer();

    // xTaskCreate(
    //     TestTask,
    //     "TestTask",
    //     4096,
    //     this,
    //     configMAX_PRIORITIES / 2,
    //     nullptr
    // );
}

void PdcInterface::EnableBuzzer()
{
    gpio_set_level(_buzzerGpioPin, 0);
}

void PdcInterface::DisableBuzzer()
{
    gpio_set_level(_buzzerGpioPin, 1);
}
void PdcInterface::TestTask(void* pvParameters)
{
    static bool enable = false;
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
        if (enable)
        {
            ((PdcInterface*)pvParameters)->EnableBuzzer();
            enable = false;
        }
        else
        {
            ((PdcInterface*)pvParameters)->DisableBuzzer();
            enable = true;
        }
    }
}
