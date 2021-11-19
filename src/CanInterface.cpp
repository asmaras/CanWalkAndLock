#include "CanInterface.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <memory.h>

CanInterface::CanInterface()
{
    
}

void CanInterface::Start()
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
    g_config.intr_flags |= ESP_INTR_FLAG_IRAM;
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t result;
    result = twai_driver_install(&g_config, &t_config, &f_config);
    printf("Installed TWAI driver, result=%d\n", result);

    result = twai_start();
    printf("Started TWAI driver, result=%d\n", result);

    xTaskCreate(
        CanReceiveTask,
        "CanReceiveTask",
        4096,
        this,
        configMAX_PRIORITIES / 2,
        nullptr
    );
}

void CanInterface::SendMessage(int id, int length, const unsigned char* data)
{
    esp_err_t result;
    twai_message_t message = {};
    message.identifier = id;
    message.data_length_code = length;
    memcpy(message.data, data, length);
    result = twai_transmit(&message, 0);
    if (result != ESP_OK)
    {
        printf("twai_transmit failed with result %d\n", result);
    }
}

void CanInterface::CanReceiveTask(void* pvParameters)
{
    esp_err_t result;
    twai_message_t message;
    while (true)
    {
        result = twai_receive(&message, portMAX_DELAY);
        if(result == ESP_OK)
        {
            // printf("ID is %03x\n", message.identifier);
            if (!(message.rtr))
            {
                // for (int i = 0; i < message.data_length_code; i++)
                // {
                //     printf("Data byte %d = %02x\n", i, message.data[i]);
                // }
                ((CanInterface*)pvParameters)->_iCanWalProcessingAdapter->HandleCanMessage(message.identifier, message.data_length_code, message.data);
            }
        }
        else
        {
            printf("twai_receive failed with result %d\n", result);
        }
    }
}
