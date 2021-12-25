#include "OsAbstraction.h"
#include "CanInterface.h"
#include "SoundPlayer.h"
#include "CanWalProcessing/CanWalProcessingAdapter.h"
#include "CanWalProcessing/CanWalProcessing.h"
#include "PdcInterface.h"
#include "CanPdcProcessing/CanPdcProcessingAdapter.h"
#include "CanPdcProcessing/CanPdcProcessing.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>

SemaphoreHandle_t processingMutex = xSemaphoreCreateMutex();
OsAbstraction::CommonHandling OsAbstraction::_commonHandling(processingMutex);
CanInterface::CommonHandling CanInterface::_commonHandling(processingMutex);
SoundPlayer soundPlayer;
CanWalProcessingAdapter canWalProcessingAdapter;
CanWalProcessing canWalProcessing;
CanPdcProcessingAdapter canPdcProcessingAdapter;
CanPdcProcessing canPdcProcessing;

void CppMain()
{
    // OS -> CanWal processing
    // CAN input messages -> CanWal processing
    canWalProcessingAdapter.SetOutputInterfaces(&canWalProcessing, &canWalProcessing);
    // CanWal processing -> OS
    // CanWal processing -> CAN output messages
    // CanWal processing -> sound playing
    canWalProcessing.SetOutputInterfaces(&canWalProcessingAdapter, &canWalProcessingAdapter, &soundPlayer);
    // CAN input messages -> CanPdc processing
    canPdcProcessingAdapter.SetOutputInterface(&canPdcProcessing);
    // CanPdc processing -> OS
    // CanPdc processing -> PDC control
    canPdcProcessing.SetOutputInterfaces(&canPdcProcessingAdapter, &canPdcProcessingAdapter);
    // Start all the things
    CanInterface::Start();

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 1ULL << GPIO_NUM_2;
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);
    int count = 0;
    while (true)
    {
        gpio_set_level(GPIO_NUM_2, (count + 1) % 2);
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