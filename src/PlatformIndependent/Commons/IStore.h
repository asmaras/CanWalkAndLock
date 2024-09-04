#pragma once

namespace PlatformIndependent::Commons
{
    class IStore
    {
    public:
        class Output
        {
        public:
            virtual bool IPiStoreGetEnableWAL() = 0;
            virtual void IPiStoreSetEnableWAL(bool enable) = 0;
            virtual bool IPiStoreGetMayCloseWindowsAndRoof() = 0;
            virtual void IPiStoreSetMayCloseWindowsAndRoof(bool enable) = 0;
        };
    };
}