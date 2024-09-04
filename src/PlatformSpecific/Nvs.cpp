#include "Nvs.h"

#include <nvs_flash.h>

namespace PlatformSpecific
{
    Nvs::Nvs()
    {
        esp_err_t result = nvs_flash_init();
        if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            // NVS partition was truncated and needs to be erased
            ESP_ERROR_CHECK(nvs_flash_erase());

            // Retry nvs_flash_init
            ESP_ERROR_CHECK(nvs_flash_init());
        }
        else
        {
            ESP_ERROR_CHECK(result);
        }
    }

    PlatformSpecific::INvs::Input::NvsHandle Nvs::IPsNvsOpen(const char(&namespaceName)[PlatformSpecific::INvs::Input::namespaceNameLength])
    {
        nvs_handle_t handle;
        ESP_ERROR_CHECK(nvs_open(namespaceName, NVS_READWRITE, &handle));
        return handle;
    }

    bool Nvs::IPsNvsGetBool(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], bool defaultValue)
    {
        unsigned char value;
        esp_err_t result = nvs_get_u8(handle, key, &value);
        assert(result == ESP_OK || result == ESP_ERR_NVS_NOT_FOUND);
        if (result == ESP_OK)
        {
            return value == 1 ? true : false;
        }
        else
        {
            return defaultValue;
        }
    }
    
    void Nvs::IPsNvsSetBool(const NvsHandle handle, const char(&key)[PlatformSpecific::INvs::Input::keyLength], bool value)
    {
        ESP_ERROR_CHECK(nvs_set_u8(handle, key, value ? 1 : 0));
        ESP_ERROR_CHECK(nvs_commit(handle));
    }
}