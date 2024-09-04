#include "CanWalkAndLock.h"
#include "PlatformSpecific/Nvs.h"
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
#include <freertos/task.h>
#include <driver/gpio.h>

namespace Pins {
    // CAN
    constexpr gpio_num_t canTx = GPIO_NUM_21;
    constexpr gpio_num_t canRx = GPIO_NUM_22;
    // Remote control
    constexpr gpio_num_t remoteControlPower = GPIO_NUM_12;
    constexpr gpio_num_t remoteControlLock = GPIO_NUM_13;
    // Sound
    constexpr gpio_num_t i2sBck = GPIO_NUM_4;
    constexpr gpio_num_t i2sWs = GPIO_NUM_5;
    constexpr gpio_num_t i2sDo = GPIO_NUM_18;
    // PDC
    constexpr gpio_num_t pdcBuzzer = GPIO_NUM_32;
    // LED indicator
    constexpr gpio_num_t ledIndicator = GPIO_NUM_2;
};

SemaphoreHandle_t processingMutex = xSemaphoreCreateMutex();
PlatformSpecific::Nvs psNvs;
PlatformSpecific::OsAbstraction psOsAbstraction(processingMutex);
PlatformSpecific::CanInterface psCanInterface(Pins::canTx, Pins::canRx, processingMutex);
PlatformSpecific::RemoteControl psRemoteControl(Pins::remoteControlPower, Pins::remoteControlLock, processingMutex);
PlatformSpecific::SoundPlayer psSoundPlayer(Pins::i2sBck, Pins::i2sWs, Pins::i2sDo);
CanWalProcessingAdapter canWalProcessingAdapter;
PlatformIndependent::CanWalProcessing piCanWalProcessing(true);
PlatformSpecific::PdcInterface psPdcInterface(Pins::pdcBuzzer);
CanPdcProcessingAdapter canPdcProcessingAdapter;
PlatformIndependent::CanPdcProcessing piCanPdcProcessing;

void CanWalkAndLock::Setup() {
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
        &psNvs,
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
    canWalProcessingAdapter.Start();
    piCanWalProcessing.Start();
    psCanInterface.Start();

    xTaskCreate(
        AliveLedTask,
        "AliveLedTask",
        4096,
        this,
        configMAX_PRIORITIES / 2,
        nullptr
    );
}

void CanWalkAndLock::Loop() {
    
}

void CanWalkAndLock::AliveLedTask(void* pvParameters) {
    ((CanWalkAndLock*)pvParameters)->AliveLedTask();
}

void CanWalkAndLock::AliveLedTask() {
    gpio_config_t gpioConfig = {};
    gpioConfig.pin_bit_mask = 1ULL << Pins::ledIndicator;
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpio_config(&gpioConfig);
    int count = 0;
    while (true)
    {
        gpio_set_level(Pins::ledIndicator, (count + 1) % 2);
        vTaskDelay(pdMS_TO_TICKS(100));
        count++;
    }
}