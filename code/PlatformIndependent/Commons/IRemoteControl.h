#pragma once

namespace PlatformIndependent::Commons
{
    class IRemoteControl
    {
    public:
        class Input
        {
        public:
            // Function calls to input interfaces should always be mutually excluded
            virtual void IPiRemoteControlOperationFinished() = 0;
        };

        class Output
        {
        public:
            virtual void IPiRemoteControlLockSinglePress() = 0;
            virtual void IPiRemoteControlLockDoublePress() = 0;
        };
    };
}