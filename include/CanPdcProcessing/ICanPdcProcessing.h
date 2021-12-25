#pragma once

class ICanPdcProcessing
{
public:
    class Os
    {
    public:
        class Output
        {
        public:
            virtual void Trace(const char* text, ...) = 0;
        };
    };

    class Can
    {
    public:
        class Input
        {
        public:
            // Function calls to input interfaces should always be mutually excluded
            virtual void HandleCanMessage(int id, int length, const unsigned char* data) = 0;
        };

        class Output
        {
        public:
            virtual void SendCanMessage(int id, int length, const unsigned char* data) = 0;
        };
    };

    class Pdc
    {
    public:
        class Output
        {
        public:
            virtual void EnableBuzzer() = 0;
            virtual void DisableBuzzer() = 0;
        };
    };
};
