#pragma once

namespace PlatformSpecific
{
    class ISoundPlayer
    {
    public:
        class Input
        {
        public:
            enum class Sound
            {
                goFast,
                goSlow,
                goFastWindowsNotClosed,
                goSlowWindowsNotCLosed,
                intermediateCountdown
            };

            // This function should play a sound immediately and interrupt a sound being played, if any
            virtual void IpsSoundPlayerPlaySound(PlatformSpecific::ISoundPlayer::Input::Sound sound) = 0;
            virtual void IpsSoundPlayerStopPlayingSound() = 0;
        };
    };
}