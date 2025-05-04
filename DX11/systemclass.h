/*

Flow
- Initialize
	- InitializeWindows
- Run (message loop)
	- message 에 따라 렌더링 (Frame) 또는 종료 등 처리
- Shutdown
	- ShutdownWindows
*/

#pragma once

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "inputclass.h"
#include "applicationclass.h"


class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	// 포인터 등 자원 해제
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};

// 윈도우 메세지 처리 함수
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 전역변수 인스턴스
static SystemClass* ApplicationHandle = 0;


#endif