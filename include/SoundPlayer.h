#pragma once

#include "CanWalProcessing/ICanWalProcessing.h"

class SoundPlayer : public ICanWalProcessing::Sound::Output
{
public:
    SoundPlayer();

    virtual void PlaySound(Sound sound) override;
    virtual void StopPlayingSound() override;

private:
    static void PlayTaskCode(void* pvParameters);
};