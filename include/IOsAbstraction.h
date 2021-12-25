#pragma once

class IOsAbstraction
{
public:
    class Output
    {
    public:
        virtual void HandleExpiredTimer(int timerId) = 0;
    };
};