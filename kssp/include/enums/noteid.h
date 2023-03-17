#pragma once

enum NoteID {
    MotorStatus = 0x01,
    PlaybackStatus = 0x02,
    ErrorStatus = 0x03,
    MotorPosition = 0x07,
    MotorCalibrated = 0x11,
    AuxInputStatus = 0x15,
    UnsupportedMessage = 0xFF,
};