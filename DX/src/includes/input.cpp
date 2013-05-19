#include "input.h"

bool keyPressed(UINT keyCode, BYTE prevKeyState[256], BYTE currKeyState[256]) {
	bool state = false;
	bool prevKey = prevKeyState[keyCode] & 0x80;
	bool currKey = currKeyState[keyCode] & 0x80;
	if (!prevKey & currKey)
		state = true;

	return state;
}

bool keyReleased(UINT keyCode, BYTE prevKeyState[256], BYTE currKeyState[256]) {
	bool state = false;
	bool prevKey = prevKeyState[keyCode] & 0x80;
	bool currKey = currKeyState[keyCode] & 0x80;
	if (prevKey & !currKey)
		state = true;

	return state;
}

bool keyDown(UINT keyCode, BYTE currKeyState[256]) {
	return (currKeyState[keyCode] & 0x80);
}

bool keyUp(UINT keyCode, BYTE currKeyState[256]) {
	return !(currKeyState[keyCode] & 0x80);
}
bool mousePressed(UINT keyCode, DIMOUSESTATE2 prevMouseState, DIMOUSESTATE2 currMouseState) {
	bool state = false;
	bool prevKey = prevMouseState.rgbButtons[keyCode] & 0x80;
	bool currKey = currMouseState.rgbButtons[keyCode] & 0x80;
	if (!prevKey & currKey)
		state = true;
	return state;
}

bool mouseReleased(UINT keyCode, DIMOUSESTATE2 prevMouseState, DIMOUSESTATE2 currMouseState) {
	bool state = false;
	bool prevKey = prevMouseState.rgbButtons[keyCode] & 0x80;
	bool currKey = currMouseState.rgbButtons[keyCode] & 0x80;
	if (prevKey & !currKey)
		state = true;
	return state;
}

