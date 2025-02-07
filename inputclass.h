////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();
	bool IsUpArrowPressed();
	bool IsDownArrowPressed();
	bool IsKeyPressed(unsigned char ch) {
		int code = 0;
		switch (ch) {
		case 'q': case 'Q': code = DIK_Q; break;
		case 'w': case 'W': code = DIK_W; break;
		case 'e': case 'E': code = DIK_E; break;
		case 'a': case 'A': code = DIK_A; break;
		case 's': case 'S': code = DIK_S; break;
		case 'd': case 'D': code = DIK_D; break;
		}
		return ( (m_keyboardState[code] & 0x80) != 0);
	}
	//bool IsKeyPressed(unsigned char ch) { return (m_keyboardState[ch] > 0); }
	void GetMouseLocation(int&, int&);
	bool IsMousePressed();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	int m_screenWidth, m_screenHeight, m_mouseX, m_mouseY;
};

#endif