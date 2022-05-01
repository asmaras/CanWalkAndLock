#pragma once

#include "PlatformSpecific/ISoundPlayer.h"

namespace PlatformSpecific
{
    class SoundPlayer : public PlatformSpecific::ISoundPlayer::Input
    {
    public:
        SoundPlayer();

    private:
        // PlatformSpecific::ISoundPlayer::Input
        void IpsSoundPlayerPlaySound(PlatformSpecific::ISoundPlayer::Input::Sound sound) override;
        void IpsSoundPlayerStopPlayingSound() override;

        static void PlayTask(void* pvParameters);
    };
}