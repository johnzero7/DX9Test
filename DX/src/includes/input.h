#ifndef INPUT_H
#define INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#define WINVER 0x0601

#include <Windows.h>
#include <dinput.h>

bool keyPressed(UINT keyCode, BYTE prevKeyState[256], BYTE currKeyState[256]);
bool keyReleased(UINT keyCode, BYTE prevKeyState[256], BYTE currKeyState[256]);
bool keyDown(UINT keyCode, BYTE currKeyState[256]);
bool keyUp(UINT keyCode, BYTE currKeyState[256]);
bool mousePressed(UINT keyCode, DIMOUSESTATE2 prevMouseState, DIMOUSESTATE2 currMouseState);
bool mouseReleased(UINT keyCode, DIMOUSESTATE2 prevMouseState, DIMOUSESTATE2 currMouseState);

#endif
