#pragma once

enum MessageID {
    NetworkInfo = 0x02,
    LEDStatus = 0x07,
    MotorInfo = 0x0B,
    DeviceInfo = 0x0F,
    UserPassword = 0x10,
    DevicePassword = 0x11,
    TrajectoryData = 0x12,
    DeviceGUID = 0x1F,
    SoftwareID = 0x26,
    Action = 0x80,
    Notification = 0x81
};

static const MessageID AllMessageIDs[] = {
        NetworkInfo,
        LEDStatus,
        MotorInfo,
        DeviceInfo,
        UserPassword,
        DevicePassword,
        TrajectoryData,
        DeviceGUID,
        SoftwareID,
        Action,
        Notification
};