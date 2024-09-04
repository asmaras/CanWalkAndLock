#include "SoundPlayer.h"
#include <driver/gpio.h>
//#include <driver/i2s_std.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#ifdef USE_ESP32_FRAMEWORK_ARDUINO
// Enable ESP_LOGX macros for logging when built with ESPHome
#include "esphome/core/log.h"
using namespace esphome;
#else
// Note: to enable ESP_LOGX macros for logging when built with PlatformIO, add build flag -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_DEBUG
#include <esp_log.h>
#endif

namespace PlatformSpecific
{
    SoundPlayer::SoundPlayer(gpio_num_t i2sBckPin, gpio_num_t i2sWsPin, gpio_num_t i2sDoPin) :
        _i2sBckPin(i2sBckPin),
        _i2sWsPin(i2sWsPin),
        _i2sDoPin(i2sDoPin)
    {
        _currentSound = {};
        _playTaskHandle = {};

        // /* Get the default channel configuration by helper macro.
        //  * This helper macro is defined in 'i2s_common.h' and shared by all the i2s communication mode.
        //  * It can help to specify the I2S role, and port id */
        // i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
        // /* Allocate a new tx channel and get the handle of this channel */
        // i2s_new_channel(&chan_cfg, &_tx_handle, NULL);

        // i2s_std_config_t std_cfg = {
        //     .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        //     .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        //     .gpio_cfg = {
        //         .mclk = I2S_GPIO_UNUSED,
        //         .bclk = _i2sBckPin,
        //         .ws = _i2sWsPin,
        //         .dout = _i2sDoPin,
        //         .din = I2S_GPIO_UNUSED,
        //         .invert_flags = {
        //             .mclk_inv = false,
        //             .bclk_inv = false,
        //             .ws_inv = false,
        //         },
        //     },
        // };
        // std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;

        // i2s_channel_init_std_mode(_tx_handle, &std_cfg);
    }

    void SoundPlayer::IpsSoundPlayerPlaySound(const short* samples, int numberOfSamples)
    {
        if (_playTaskHandle != nullptr)
        {
            IpsSoundPlayerStopPlayingSound();
        }

        // i2s_config_t i2sConfig;
        // i2sConfig.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        // sample_rate, bits_per_sample and channel_format will be overwritten by i2s_set_clk
        // Just set some valid values
        // i2sConfig.sample_rate = 44100;
        // i2sConfig.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        // i2sConfig.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        // i2sConfig.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        // i2sConfig.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        // i2sConfig.dma_buf_count = 2;
        // i2sConfig.dma_buf_len = 1000;
        // i2sConfig.use_apll = false;
        // i2sConfig.tx_desc_auto_clear = true;
        // i2sConfig.fixed_mclk = 0;
        // i2sConfig.mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT;
        // i2sConfig.bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT;
        // i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, NULL);

        // i2s_pin_config_t i2sPinConfig;
        // i2sPinConfig.mck_io_num = -1;
        // i2sPinConfig.bck_io_num = _i2sBckPin;
        // i2sPinConfig.ws_io_num = _i2sWsPin;
        // i2sPinConfig.data_out_num = _i2sDoPin;
        // i2sPinConfig.data_in_num = -1;
        // i2s_set_pin(I2S_NUM_0, &i2sPinConfig);

        // // Source is mono
        // i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

        // _currentSound.tx_handle = _tx_handle;
        _currentSound.samples = samples;
        _currentSound.numberOfSamples = numberOfSamples;

        /* Before write data, start the tx channel first */
        // i2s_channel_enable(_tx_handle);

        // xTaskCreate(
        //     PlayTask,
        //     "Play",
        //     4096,
        //     &_currentSound,
        //     configMAX_PRIORITIES / 2,
        //     &_playTaskHandle
        // );
    }

    void SoundPlayer::IpsSoundPlayerStopPlayingSound()
    {
        vTaskDelete(_playTaskHandle);
        _playTaskHandle = {};
        // i2s_channel_disable(_tx_handle);
    }

    void SoundPlayer::PlayTask(void* pvParameters)
    {
        CurrentSound* currentSound = (CurrentSound*)pvParameters;
        ESP_LOGI(_logTag, "Playing %d samples", currentSound->numberOfSamples);
        size_t bytesWritten;
        // i2s_write(I2S_NUM_0, currentSound->samples, currentSound->numberOfSamples * 2, &bytesWritten, portMAX_DELAY);
        // // Even though tx_desc_auto_clear is set, some noise is heard after all samples have been written
        // To avoid the noise, zero the DMA buffer
        // i2s_zero_dma_buffer(I2S_NUM_0);
        
        // i2s_channel_write(currentSound->tx_handle, currentSound->samples, currentSound->numberOfSamples * 2, &bytesWritten, portMAX_DELAY);
        // ESP_LOGI(_logTag, "Play completed");
        // vTaskDelay(portMAX_DELAY);
    }
}