#include "input.h"
#include "keyboard.h"
#include <set>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <xinput.h>
#pragma comment (lib, "xinput.lib")


static INPUT_STATE currentInputs;
static INPUT_STATE lastInputs;

static bool xInputConnected = false;

static std::unordered_map<std::wstring, std::wstring> pcLabels;
static std::unordered_map<std::wstring, std::wstring> xinputLabels;

void InitInput() {

    pcLabels = {
        {L"↑", L"[W]"},
        {L"↓", L"[S]"},
        {L"←", L"[A]"},
        {L"→", L"[D]"},
        {L"{AnalogLeft}", L"[A][D]"},
        {L"{OK}", L"[Enter]"},
        {L"{Cancel}", L"[Back]"},
        {L"{Start}", L"[Space]"},
        {L"{Select}", L"[Z]"}
    };
    xinputLabels = {
        {L"{AnalogLeft}", L"[左スティック]"},
        {L"{OK}", L"🅰"},
        {L"{Cancel}", L"🅱"},
        {L"{Start}", L"[Start]"},
        {L"{Select}", L"[Back]"}
    };
}

void UnitInput() {

}

bool HasXInpput()
{
    return xInputConnected;
}

std::wstring GetInputLabel(const std::wstring& placeholder)
{
    if (xInputConnected) {
        return (xinputLabels.count(placeholder)) ? xinputLabels[placeholder] : placeholder;
    }
    return (pcLabels.count(placeholder)) ? pcLabels[placeholder] : placeholder;
}

void UpdateInput()
{
    memcpy(&lastInputs, &currentInputs, sizeof(INPUT_STATE));
    memset(&currentInputs, 0, sizeof(INPUT_STATE));

    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    DWORD dwResult = XInputGetState(0, &state);

    if (dwResult == ERROR_SUCCESS) {
        xInputConnected = true;

        currentInputs.buttons[BUTTON_STATE_0] = currentInputs.buttons[BUTTON_STATE_0] |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) << 1 |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) << 2 |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) << 3 |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_A)) << 4 |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_B)) << 5 |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_START)) << 6 |
            (bool)((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)) << 7;

        if (state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            currentInputs.analog[ANALOG_STATE_LEFT_X] = (float)state.Gamepad.sThumbLX / 32767.0f;
        }
        else if(state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) 
        {
            currentInputs.analog[ANALOG_STATE_LEFT_X] = (float)state.Gamepad.sThumbLX / 32768.0f;
        }
        else {
            currentInputs.analog[ANALOG_STATE_LEFT_X] = 0.0f;
        }

        if (state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            currentInputs.analog[ANALOG_STATE_LEFT_Y] = (float)state.Gamepad.sThumbLY / 32767.0f;
        }
        else if (state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            currentInputs.analog[ANALOG_STATE_LEFT_Y] = (float)state.Gamepad.sThumbLY / 32768.0f;
        }
        else {
            currentInputs.analog[ANALOG_STATE_LEFT_Y] = 0.0f;
        }
    }
    else {
        xInputConnected = false;
    }

    currentInputs.buttons[BUTTON_STATE_0] = currentInputs.buttons[BUTTON_STATE_0] |
        Keyboard_IsKeyDown(KK_W) |
        Keyboard_IsKeyDown(KK_S) << 1 |
        Keyboard_IsKeyDown(KK_A) << 2 |
        Keyboard_IsKeyDown(KK_D) << 3 |
        Keyboard_IsKeyDown(KK_ENTER) << 4 |
        Keyboard_IsKeyDown(KK_BACK) << 5 |
        Keyboard_IsKeyDown(KK_SPACE) << 6 |
        Keyboard_IsKeyDown(KK_Z) << 7;

}

bool IsInputDown(const BUTTON_STATE alias, const unsigned int mask)
{
    if (alias < 0 || alias >= BUTTON_STATE_MAX) return false;
    return currentInputs.buttons[alias] & mask;
}

bool IsInputTrigger(const BUTTON_STATE alias, const unsigned int mask)
{
    if (alias < 0 || alias >= BUTTON_STATE_MAX) return false;

    return (currentInputs.buttons[alias] & mask) && !(lastInputs.buttons[alias] & mask);
}

float GetInputAnalogValue(const ANALOG_STATE alias)
{
    return currentInputs.analog[alias];
}



