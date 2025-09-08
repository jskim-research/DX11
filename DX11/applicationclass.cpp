#include "applicationclass.h"
#include <fstream>

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
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

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_Model = new ModelClass;
	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShaderClass;
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
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

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.0f, 1.0f);  // yellow

	m_Camera->Render();  // view matrix °è»ê

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}
