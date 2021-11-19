#pragma once

class ICanWalProcessingAdapter
{
public:
    virtual void HandleCanMessage(int id, int length, const unsigned char* data) = 0;
};
