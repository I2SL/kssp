#include <gainput/gainput.h>

#include "../libkessler/stage.h"

enum Button
{
    TiltUp,
    TiltDown,
    SpeedUp,
    SpeedDown,
    PanRight,
    PanLeft,
    SlideRight,
    SlideLeft,
    Stop
};

int main() {
    gainput::InputManager manager;
    manager.SetDisplaySize(1920, 1080);
    const gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();

    gainput::InputMap map(manager);
    map.MapBool(Stop, keyboardId, gainput::KeyEscape);
    map.MapBool(TiltUp, keyboardId, gainput::KeyW);
    map.MapBool(TiltDown, keyboardId, gainput::KeyS);

    bool running = true;

    while (running) {
        manager.Update();
        //printf("Hello I am running\n");
        if (map.GetBoolWasDown(TiltUp))
        {
            running = false;
            fmt::print("I should be dead now\n");
        }
    }
}