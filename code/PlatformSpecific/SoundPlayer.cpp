#include "SoundPlayer.h"
#include "driver/i2s.h"

namespace PlatformSpecific
{
    SoundPlayer::SoundPlayer()
    {

    }

    void SoundPlayer::IpsSoundPlayerPlaySound(PlatformSpecific::ISoundPlayer::Input::Sound sound)
    {
        i2s_config_t i2s_config;
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        // sample_rate, bits_per_sample and channel_format will be overwritten by i2s_set_clk
        // Just set some valid values
        i2s_config.sample_rate = 44100;
        i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        i2s_config.dma_buf_count = 2;
        i2s_config.dma_buf_len = 100;
        i2s_config.use_apll = false;
        i2s_config.tx_desc_auto_clear = false;
        i2s_config.fixed_mclk = 0;
        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

        i2s_pin_config_t i2s_pin_config;
        i2s_pin_config.bck_io_num = GPIO_NUM_4;
        i2s_pin_config.ws_io_num = GPIO_NUM_5;
        i2s_pin_config.data_out_num = GPIO_NUM_18;
        i2s_pin_config.data_in_num = -1;
        i2s_set_pin(I2S_NUM_0, &i2s_pin_config);

        i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO); // Source is mono

        // xTaskCreate(
        //     PlayTaskCode,
        //     "Play",
        //     4096,
        //     nullptr,
        //     1,
        //     &_playTaskHandle
        // );

        // printf("deleting task\n");
        // vTaskDelete(_playTaskHandle);
        // i2s_driver_uninstall(I2S_NUM_0);
        // Play();
    }

    void SoundPlayer::IpsSoundPlayerStopPlayingSound()
    {

    }

    void SoundPlayer::PlayTask(void* pvParameters)
    {
        // printf("Play started\n");
        // size_t bytes_written;
        // i2s_write(I2S_NUM_0, audioFragment, sizeof(audioFragment), &bytes_written, pdMS_TO_TICKS(100));
        // i2s_zero_dma_buffer(I2S_NUM_0);
        // vTaskDelay(portMAX_DELAY);
    }
}