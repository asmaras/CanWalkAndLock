#include "RemoteControl.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace PlatformSpecific
{
    RemoteControl::RemoteControl(SemaphoreHandle_t& processingMutex) :
        _processingMutex(processingMutex)
    {
        _iOutput = nullptr;
        
        // Prime outputs for the remote control
        // Power: off(0)
        // Lock button position: released(1)
        gpio_config_t io_conf = {};
        io_conf.pin_bit_mask = 1ULL << _powerGpioPin;
        io_conf.mode = GPIO_MODE_OUTPUT;
        gpio_config(&io_conf);
        gpio_set_level(_powerGpioPin, 0);
        io_conf.pin_bit_mask = 1ULL << _lockGpioPin;
        io_conf.mode = GPIO_MODE_OUTPUT;
        gpio_config(&io_conf);
        gpio_set_level(_lockGpioPin, 1);
        
        xTaskCreate(
            RemoteControlTask,
            "RemoteControlTask",
            4096,
            this,
            configMAX_PRIORITIES / 2,
            &_remoteControlTaskHandle
        );
    }

    void RemoteControl::IPsRemoteControlLockSinglePress(PlatformSpecific::IRemoteControl::Output* iOutput)
    {
        if (_iOutput == nullptr)
        {
            _iOutput = iOutput;
            xTaskNotify(_remoteControlTaskHandle, NotificationValues::singlePress, eSetBits);
        }
    }

    void RemoteControl::IPsRemoteControlLockDoublePress(PlatformSpecific::IRemoteControl::Output * iOutput)
    {
        if (_iOutput == nullptr)
        {
            _iOutput = iOutput;
            xTaskNotify(_remoteControlTaskHandle, NotificationValues::doublePress, eSetBits);
        }
    }

    void RemoteControl::RemoteControlTask(void* pvParameters)
    {
        RemoteControl* remoteControl = (RemoteControl*)pvParameters;
        uint32_t notificationValue;
        while (true)
        {
            xTaskNotifyWait(
                0,
                ULONG_MAX,
                &notificationValue,
                portMAX_DELAY
            );

            // Step 1:
            // Power: on(1)
            gpio_set_level(_powerGpioPin, 1);
            // Wait Timings::powerUp to power up the remote control
            vTaskDelay(pdMS_TO_TICKS(Timings::powerUp));
            // Step 2:
            // Lock button position: depressed(0)
            gpio_set_level(_lockGpioPin, 0);
            // Keep the button in depressed state for Timings::buttonPress
            vTaskDelay(pdMS_TO_TICKS(Timings::buttonPress));
            // Step 3:
            // Lock button position: released(1)
            gpio_set_level(_lockGpioPin, 1);
            // Wait Timings::buttonRelease for the button release signal to be transmitted
            vTaskDelay(pdMS_TO_TICKS(Timings::buttonRelease));
            if ((notificationValue & NotificationValues::doublePress) != 0)
            {
                // Step 3a:
                // Lock button position: depressed(0)
                gpio_set_level(_lockGpioPin, 0);
                // Keep the button in depressed state for Timings::buttonPress
                vTaskDelay(pdMS_TO_TICKS(Timings::buttonPress));
                // Step 3b:
                // Lock button position: released(1)
                gpio_set_level(_lockGpioPin, 1);
                // Wait Timings::buttonRelease for the button release signal to be transmitted
                vTaskDelay(pdMS_TO_TICKS(Timings::buttonRelease));
            }
            // Step 4:
            // Power off: (0)
            gpio_set_level(_powerGpioPin, 0);

            // Notify the user that the operation has finished
            xSemaphoreTake(remoteControl->_processingMutex, portMAX_DELAY);
            remoteControl->_iOutput->IPsRemoteControlOperationFinished();
            remoteControl->_iOutput = nullptr;
            xSemaphoreGive(remoteControl->_processingMutex);
        }
    }
}