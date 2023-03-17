#pragma once

enum MessageType {
    Set = 0x00,
    Get = 0x01,
    Response = 0x02,
    Ongoing = 0x82
};