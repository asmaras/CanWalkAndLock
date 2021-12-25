#pragma once

class ICanInterface
{
public:
    class Output
    {
    public:
        virtual void HandleCanMessage(int id, int length, const unsigned char* data) = 0;
    };
};