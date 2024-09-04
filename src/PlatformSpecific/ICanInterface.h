#pragma once

namespace PlatformSpecific
{
    class ICanInterface
    {
    public:
        class Input
        {
        public:
            virtual void IpsCanInterfaceSendCanMessage(int id, int length, const unsigned char* data) = 0;
        };

        class Output
        {
        public:
            virtual void IPsCanInterfaceHandleCanMessage(int id, int length, const unsigned char* data) = 0;
        };
    };
}