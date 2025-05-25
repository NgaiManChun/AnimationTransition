#ifndef _INPUT_H
#define _INPUT_H

#define INPUT_UP		BUTTON_STATE_0, 0x01
#define INPUT_DOWN		BUTTON_STATE_0, 0x02
#define INPUT_LEFT		BUTTON_STATE_0, 0x04
#define INPUT_RIGHT		BUTTON_STATE_0, 0x08
#define INPUT_OK		BUTTON_STATE_0, 0x10
#define INPUT_CANCEL	BUTTON_STATE_0, 0x20
#define INPUT_START		BUTTON_STATE_0, 0x40
#define INPUT_SELECT	BUTTON_STATE_0, 0x80

#include <string>

enum BUTTON_STATE {
	BUTTON_STATE_0 = 0,
	BUTTON_STATE_MAX
};

enum ANALOG_STATE {
	ANALOG_STATE_LEFT_X = 0,
	ANALOG_STATE_LEFT_Y,
	ANALOG_STATE_ACCE_X,
	ANALOG_STATE_ACCE_Y,
	ANALOG_STATE_ACCE_Z,
	ANALOG_STATE_MAX
};

struct INPUT_STATE {
	unsigned int buttons[BUTTON_STATE_MAX];
	float analog[ANALOG_STATE_MAX];
};

void InitInput();
void UnitInput();
void UpdateInput();
bool IsInputDown(const BUTTON_STATE alias, const unsigned int mask);
bool IsInputTrigger(const BUTTON_STATE alias, const unsigned int mask);
float GetInputAnalogValue(const ANALOG_STATE alias);
bool HasXInpput();
std::wstring GetInputLabel(const std::wstring& placeholder);

#endif