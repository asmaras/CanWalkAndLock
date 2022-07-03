#pragma once

namespace PlatformIndependent::Commons
{
    class ICan
    {
    public:
        // CAN IDs
        enum class CanId
        {
            frontPassengerSideDoorStatus = 0x0E2,
            rearPassengerSideDoorStatus = 0x0E6,
            frontDriverSideDoorStatus = 0x0EA,
            rearDriverSideDoorStatus = 0x0EE,
            mirrorFoldStatus = 0x0F6,
            ignitionAndKeyStatus = 0x130,
            vehicleSpeed = 0x1B4,
            gearShifterPosition = 0x1D2,
            remoteControlAndDoorHandleInput = 0x23A,
            windowRoofAndMirrorControl = 0x26E,
            doorLockControl = 0x2A0,
            dateTime = 0x2F8,
            seatbeltAndSeatOccupancyStatus = 0x2FA,
            doorStatus = 0x2FC,
            handbrakeStatus = 0x34F,
            setDateTime = 0x39E
        };

        class Input
        {
        public:
            // Function calls to input interfaces should always be mutually excluded
            virtual void IPiCanHandleCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data) = 0;
        };

        class Output
        {
        public:
            virtual void IPiCanSendCanMessage(PlatformIndependent::Commons::ICan::CanId id, int length, const unsigned char* data) = 0;
        };
    };
}