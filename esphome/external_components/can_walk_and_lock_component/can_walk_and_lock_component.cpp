#include "esphome/core/log.h"
#include "can_walk_and_lock_component.h"
#include "CanWalkAndLock.h"

namespace esphome {
    namespace can_walk_and_lock_component {

        static const char* TAG = "can_walk_and_lock_component.component";
        static CanWalkAndLock canWalkAndLock;

        void CanWalkAndLockComponent::setup() {
            canWalkAndLock.Setup();
        }

        void CanWalkAndLockComponent::loop() {
            canWalkAndLock.Loop();
        }

        void CanWalkAndLockComponent::dump_config() {
            ESP_LOGCONFIG(TAG, "CAN walk and lock component");
        }

    }  // namespace can_walk_and_lock_component
}  // namespace esphome