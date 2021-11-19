#pragma once

#include "ISoundPlayer.h"

class SoundPlayer : public ISoundPlayer
{
public:
    SoundPlayer();

    void Play() override;

private:
    static void PlayTaskCode(void* pvParameters);
};