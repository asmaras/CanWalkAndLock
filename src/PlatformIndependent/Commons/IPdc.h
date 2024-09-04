#pragma once

namespace PlatformIndependent::Commons
{
    class IPdc
    {
    public:
        class Output
        {
        public:
            virtual void IPiPdcEnableBuzzer() = 0;
            virtual void IPiPdcDisableBuzzer() = 0;
        };
    };
}