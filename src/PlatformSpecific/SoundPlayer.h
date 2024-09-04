#pragma once

#include "PlatformSpecific/ISoundPlayer.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <driver/i2s_std.h>

namespace PlatformSpecific
{
    class SoundPlayer : public PlatformSpecific::ISoundPlayer::Input
    {
    public:
        SoundPlayer(gpio_num_t i2sBckPin, gpio_num_t i2sWsPin, gpio_num_t i2sDoPin);

    private:
        // PlatformSpecific::ISoundPlayer::Input
        void IpsSoundPlayerPlaySound(const short* samples, int numberOfSamples) override;
        void IpsSoundPlayerStopPlayingSound() override;

        static void PlayTask(void* pvParameters);
        
        const gpio_num_t _i2sBckPin;
        const gpio_num_t _i2sWsPin;
        const gpio_num_t _i2sDoPin;
        static constexpr const char* _logTag = "SoundPlayer";
        struct CurrentSound
        {
            // i2s_chan_handle_t tx_handle;
            const short* samples;
            int numberOfSamples;
        } _currentSound;
        TaskHandle_t _playTaskHandle;
        // i2s_chan_handle_t _tx_handle;
    };
}