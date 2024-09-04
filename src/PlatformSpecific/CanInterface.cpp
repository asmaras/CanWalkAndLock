#include "CanInterface.h"
#include <driver/twai.h>
#ifdef USE_ESP32_FRAMEWORK_ARDUINO
// Enable ESP_LOGX macros for logging when built with ESPHome
#include "esphome/core/log.h"
using namespace esphome;
#else
// Note: to enable ESP_LOGX macros for logging when built with PlatformIO, add build flag -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_DEBUG
#include <esp_log.h>
#endif
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <memory.h>

namespace PlatformSpecific
{
    CanInterface::CanInterface(gpio_num_t canTxPin, gpio_num_t canRxPin, SemaphoreHandle_t& processingMutex) :
        _canTxPin(canTxPin),
        _canRxPin(canRxPin),
        _processingMutex(processingMutex)
    {

    }

    void CanInterface::AddOutputInterface(PlatformSpecific::ICanInterface::Output* iOutput)
    {
        _iOutputs.push_back(iOutput);
    }

    void CanInterface::Start()
    {
        twai_general_config_t generalConfig = TWAI_GENERAL_CONFIG_DEFAULT(_canTxPin, _canRxPin, TWAI_MODE_NORMAL);
        twai_timing_config_t timingConfig = TWAI_TIMING_CONFIG_100KBITS();
        twai_filter_config_t filterConfig = TWAI_FILTER_CONFIG_ACCEPT_ALL();

        ESP_ERROR_CHECK(twai_driver_install(&generalConfig, &timingConfig, &filterConfig));
        ESP_LOGI(_logTag, "Installed TWAI driver");

        ESP_ERROR_CHECK(twai_start());
        ESP_LOGI(_logTag, "Started TWAI driver");

        xTaskCreate(
            CanReceiveTask,
            "CanReceiveTask",
            4096,
            this,
            configMAX_PRIORITIES / 2,
            nullptr
        );
    }

    void CanInterface::IpsCanInterfaceSendCanMessage(int id, int length, const unsigned char* data)
    {
        twai_message_t message = {};
        message.identifier = id;
        message.data_length_code = length;
        memcpy(message.data, data, length);
        ESP_ERROR_CHECK_WITHOUT_ABORT(twai_transmit(&message, 0));
    }

    void CanInterface::CanReceiveTask(void* pvParameters)
    {
        CanInterface* canInterface = (CanInterface*)pvParameters;
        twai_message_t message;
        while (true)
        {
            if (ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&message, portMAX_DELAY)) == ESP_OK)
            {
                if (!(message.rtr))
                {
                    xSemaphoreTake(canInterface->_processingMutex, portMAX_DELAY);
                    for (auto const& iOutput : canInterface->_iOutputs)
                    {
                        iOutput->IPsCanInterfaceHandleCanMessage(message.identifier, message.data_length_code, message.data);
                    }
                    xSemaphoreGive(canInterface->_processingMutex);
                }
            }
        }
    }
}