enum MessageType {
    Set = 0x00,
    Get = 0x01,
    Response = 0x02
};

static const MessageType AllMessageTypes[] = {
        Set,
        Get,
        Response
};