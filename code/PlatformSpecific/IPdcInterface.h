#pragma once

namespace PlatformSpecific
{
    class IPdcInterface
    {
    public:
        class Input
        {
        public:
            virtual void IPsPdcInterfaceEnableBuzzer() = 0;
            virtual void IPsPdcInterfaceDisableBuzzer() = 0;
        };
    };
}