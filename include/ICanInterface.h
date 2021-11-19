#pragma once

class ICanInterface
{
public:
    virtual void SendMessage(int id, int length, const unsigned char* data) = 0;
};