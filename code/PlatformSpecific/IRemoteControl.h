#pragma once

namespace PlatformSpecific
{
    class IRemoteControl
    {
    public:
        class Output
        {
        public:
            virtual void IPsRemoteControlOperationFinished() = 0;
        };

        class Input
        {
        public:
            virtual void IPsRemoteControlLockSinglePress(PlatformSpecific::IRemoteControl::Output* iOutput) = 0;
            virtual void IPsRemoteControlLockDoublePress(PlatformSpecific::IRemoteControl::Output* iOutput) = 0;
        };
    };
}