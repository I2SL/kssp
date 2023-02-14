#pragma once

enum ActionID {
    ResetDevice = 0x01,
    ResetAxis = 0x02,
    MarkBeginPosition = 0x03,
    MarkEndPosition = 0x04,
    StartPlayback = 0x05,
    StopPlayback = 0x06,
    RapidToFirstKeyFrame = 0x07,
    TakePicture = 0x09,
    SetupPlayback = 0x0F,
    SetPositionSpeedAcceleration = 0x1C,
    BumpPositionSpeedAcceleration = 0x28
};

static const ActionID AllActionIDs[] = {
        ResetDevice,
        ResetAxis,
        MarkBeginPosition,
        MarkEndPosition,
        StartPlayback,
        StopPlayback,
        RapidToFirstKeyFrame,
        TakePicture,
        SetupPlayback,
        SetPositionSpeedAcceleration,
        BumpPositionSpeedAcceleration
};