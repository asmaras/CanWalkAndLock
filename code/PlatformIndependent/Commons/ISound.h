#pragma once

namespace PlatformIndependent::Commons
{
    class ISound
    {
    public:
        class Output
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
            virtual void IPiSoundPlaySound(PlatformIndependent::Commons::ISound::Output::Sound sound) = 0;
            virtual void IPiSoundStopPlayingSound() = 0;
        };
    };
}