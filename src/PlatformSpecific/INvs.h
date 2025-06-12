#pragma once

#include <nvs_flash.h>
#include <stdint.h>

namespace PlatformSpecific
{
    class INvs
    {
    public:
        class Input
        {
        public:
            static constexpr int namespaceNameLength = NVS_KEY_NAME_MAX_SIZE;
            static constexpr int keyLength = NVS_KEY_NAME_MAX_SIZE;
            using NvsHandle = nvs_handle_t;
            virtual PlatformSpecific::INvs::Input::NvsHandle IPsNvsOpen(const char(&namespaceName)[PlatformSpecific::INvs::Input::namespaceNameLength]) = 0;
            virtual bool IPsNvsGetBool(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], bool defaultValue) = 0;
            virtual void IPsNvsSetBool(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], bool value) = 0;
            virtual uint16_t IPsNvsGetUint16(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], uint16_t defaultValue) = 0;
            virtual void IPsNvsSetUint16(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], uint16_t value) = 0;
        };
    };
}