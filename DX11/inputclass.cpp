#include "inputclass.h"

InputClass::InputClass()
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
