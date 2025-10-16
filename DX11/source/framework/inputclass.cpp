#include "framework/inputclass.h"
#include <math.h>

InputClass::InputClass():
	m_isWheelDown(false),
	m_isWheelUp(false),
	m_isLButtonDown(false),
	m_prevMouseX(0),
	m_prevMouseY(0),
	m_curMouseX(0),
	m_curMouseY(0)
{

}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	int i;


	// Initialize all the keys to being released and not pressed.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}

void InputClass::Wheel(short delta)
{
	if (delta > 0) m_isWheelUp = true;
	else if (delta < 0) m_isWheelDown = true;
}

void InputClass::LButtonDown(int mouseX, int mouseY)
{
	m_isLButtonDown = true;
	m_prevMouseX = m_curMouseX = mouseX;
	m_prevMouseY = m_curMouseY = mouseY;
}

void InputClass::LButtonUp(int mouseX, int mouseY)
{
	m_isLButtonDown = false;
	m_prevMouseX = m_curMouseX = mouseX;
	m_prevMouseY = m_curMouseY = mouseY;
}

void InputClass::MouseMove(int mouseX, int mouseY)
{
	if (m_isLButtonDown)
	{
		m_prevMouseX = m_curMouseX;
		m_prevMouseY = m_curMouseY;
		m_curMouseX = mouseX;
		m_curMouseY = mouseY;
	}
}

bool InputClass::IsKeyDown(unsigned int key)
{	
	// Return what state the key is in (pressed/not pressed).

	return m_keys[key];
}

bool InputClass::IsWheelUp() const
{
	return m_isWheelUp;
}

bool InputClass::IsWheelDown() const
{
	return m_isWheelDown;
}

void InputClass::ResetWheel()
{
	m_isWheelUp = false;
	m_isWheelDown = false;
}

bool InputClass::IsLeftButtonDown() const
{
	return m_isLButtonDown;
}

int InputClass::GetMouseDeltaX() const
{
	if (abs(m_curMouseX - m_prevMouseX) > 2)
		return m_curMouseX - m_prevMouseX;
	return 0;
}

int InputClass::GetMouseDeltaY() const
{
	if (abs(m_curMouseY - m_prevMouseY) > 2)
		return m_curMouseY - m_prevMouseY;
	return 0;
}
