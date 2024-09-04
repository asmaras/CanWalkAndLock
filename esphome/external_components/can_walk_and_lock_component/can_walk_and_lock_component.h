#pragma once

#include "esphome/core/component.h"

namespace esphome {
    namespace can_walk_and_lock_component {

        class CanWalkAndLockComponent : public Component {
        public:
            void setup() override;
            void loop() override;
            void dump_config() override;
        };

    }  // namespace can_walk_and_lock_component
}  // namespace esphome