#pragma once

namespace PlatformSpecific
{
    class ISoundPlayer
    {
    public:
        class Input
        {
        public:
            // This function should play a sound immediately and interrupt a sound being played, if any
            virtual void IpsSoundPlayerPlaySound(const short* samples, int numberOfSamples) = 0;
            virtual void IpsSoundPlayerStopPlayingSound() = 0;
        };
    };
}