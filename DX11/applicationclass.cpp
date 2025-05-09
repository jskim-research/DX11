#include "applicationclass.h"
#include <fstream>

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	char* videoCardName = new char[128];
	int memory = 0;

	m_Direct3D->GetVideoCardInfo(videoCardName, memory);

	std::ofstream fout;
	fout.open("video_card_info.txt");
	fout << videoCardName << ' ' << memory << std::endl;
	fout.close();

	return true;
}

void ApplicationClass::Shutdown()
{
	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool ApplicationClass::Frame()
{
	bool result;

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}
}

bool ApplicationClass::Render()
{
	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.0f, 1.0f);  // yellow

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}
