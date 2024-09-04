#pragma once

#include "INvs.h"

namespace PlatformSpecific
{
    class Nvs : public PlatformSpecific::INvs::Input
    {
    public:
        Nvs();

        // PlatformSpecific::INvs::Input
        PlatformSpecific::INvs::Input::NvsHandle IPsNvsOpen(const char(&namespaceName)[PlatformSpecific::INvs::Input::namespaceNameLength]) override;
        bool IPsNvsGetBool(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], bool defaultValue) override;
        void IPsNvsSetBool(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], bool value) override;
    };

}