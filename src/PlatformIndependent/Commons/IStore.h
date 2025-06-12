#pragma once

#include <stdint.h>

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
            virtual uint16_t IPIStoreGetMirrorFoldFails() = 0;
            virtual void IPIStoreSetMirrorFoldFails(uint16_t fails) = 0;
            virtual uint16_t IPIStoreGetMirrorFoldRecoveries() = 0;
            virtual void IPIStoreSetMirrorFoldRecoveries(uint16_t recoveries) = 0;
        };
    };
}