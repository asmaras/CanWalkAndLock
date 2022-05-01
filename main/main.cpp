#include "PlatformSpecific/OsAbstraction.h"
#include "PlatformSpecific/CanInterface.h"
#include "PlatformSpecific/RemoteControl.h"
#include "PlatformSpecific/SoundPlayer.h"
#include "Adapters/CanWalProcessingAdapter.h"
#include "PlatformIndependent/CanWalProcessing/CanWalProcessing.h"
#include "PlatformSpecific/PdcInterface.h"
#include "Adapters/CanPdcProcessingAdapter.h"
#include "PlatformIndependent/CanPdcProcessing/CanPdcProcessing.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>

SemaphoreHandle_t processingMutex = xSemaphoreCreateMutex();
PlatformSpecific::OsAbstraction psOsAbstraction(processingMutex);
PlatformSpecific::CanInterface psCanInterface(processingMutex);
PlatformSpecific::RemoteControl psRemoteControl(processingMutex);
PlatformSpecific::SoundPlayer psSoundPlayer;
CanWalProcessingAdapter canWalProcessingAdapter;
PlatformIndependent::CanWalProcessing piCanWalProcessing(true);
PlatformSpecific::PdcInterface psPdcInterface;
CanPdcProcessingAdapter canPdcProcessingAdapter;
PlatformIndependent::CanPdcProcessing piCanPdcProcessing;

void CppMain()
{
    // Platform specific -> adapters
    //==============================
    psCanInterface.AddOutputInterface(&canWalProcessingAdapter);
    psCanInterface.AddOutputInterface(&canPdcProcessingAdapter);
    // Platform independent -> adapters
    //=================================
    piCanWalProcessing.SetOutputInterfaces(
        &canWalProcessingAdapter,
        &canWalProcessingAdapter,
        &canWalProcessingAdapter,
        &canWalProcessingAdapter
    );
    piCanPdcProcessing.SetOutputInterfaces(
        &canPdcProcessingAdapter,
        &canPdcProcessingAdapter,
        &canPdcProcessingAdapter
    );
    // Adapters -> Platform specific
    // Adapters -> Platform independent
    //=================================
    canWalProcessingAdapter.SetOutputInterfaces(
        &psOsAbstraction,
        &piCanWalProcessing,
        &psCanInterface,
        &piCanWalProcessing,
        &psRemoteControl,
        &piCanWalProcessing,
        &psSoundPlayer
        );
    canPdcProcessingAdapter.SetOutputInterfaces(
        &psOsAbstraction,
        &piCanPdcProcessing,
        &psCanInterface,
        &piCanPdcProcessing,
        &psPdcInterface
    );
    // Start all the things
    psCanInterface.Start();

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