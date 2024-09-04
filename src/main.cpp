#include <Arduino.h>
#include "CanWalkAndLock.h"

CanWalkAndLock canWalkAndLock;

void setup() {
    canWalkAndLock.Setup();
}

void loop() {
    canWalkAndLock.Loop();
}