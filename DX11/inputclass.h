/*
지금은 단순히 Message -> WndProc -> SystemClass.MessageHandler 에서 키 입력 여부 데이터 저장
*/
#pragma once

#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	void Wheel(short delta);
	void LButtonDown(int mouseX, int mouseY);
	void LButtonUp(int mouseX, int mouseY);
	void MouseMove(int mouseX, int mouseY);


	bool IsKeyDown(unsigned int);
	bool IsWheelUp() const;
	bool IsWheelDown() const;
	void ResetWheel();
	bool IsLeftButtonDown() const;
	int GetMouseDeltaX() const;
	int GetMouseDeltaY() const;

private:
	bool m_keys[256];
	bool m_isWheelUp;
	bool m_isWheelDown;
	bool m_isLButtonDown;
	int m_prevMouseX;
	int m_prevMouseY;
	int m_curMouseX;
	int m_curMouseY;
};

#endif