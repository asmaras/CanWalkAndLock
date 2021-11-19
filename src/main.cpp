#include "CanInterface.h"
#include "CanWalProcessingAdapter.h"
#include "CanWalProcessing.h"
#include "SoundPlayer.h"
#include "driver/gpio.h"

CanInterface canInterface;
SoundPlayer soundPlayer;
CanWalProcessingAdapter canWalProcessingAdapter;
CanWalProcessing canWalProcessing;

void CppMain()
{
    canInterface._iCanWalProcessingAdapter = &canWalProcessingAdapter;
    canWalProcessingAdapter._iCanWalProcessing = &canWalProcessing;
    canWalProcessingAdapter._iCanInterface = &canInterface;
    canWalProcessingAdapter._iSoundPlayer = &soundPlayer;
    canWalProcessing._iCanWalProcessingOut = &canWalProcessingAdapter;
    canInterface.Start();

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 1ULL << GPIO_NUM_0;
    io_conf.mode = GPIO_MODE_OUTPUT_OD;
    gpio_config(&io_conf);
    int count = 0;
    while (true)
    {
        gpio_set_level(GPIO_NUM_0, count % 2);
        vTaskDelay(pdMS_TO_TICKS(100));
        count++;
    }
}

extern "C"
{
    void app_main()
    {
        CppMain();
    }
}