#include "applicationclass.h"
#include "cartoonshaderinput.h"
#include <fstream>

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_DirectionalLight = 0;
	m_CartoonShader = 0;
	m_PointLights = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
	char objFileName1[128];
	char objFileName2[128];
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
	m_Camera->SetPosition(0.0f, 0.0f, -15.0f);
	m_Camera->SetRotation(0, 0, 0);

	m_Model = new ModelClass;
	strcpy_s(objFileName1, "./data/burnice.glb");
	strcpy_s(objFileName2, "./data/Cube.txt");
	strcpy_s(textureFilename, "./data/stone01.tga");
	// result = m_Model->ImportFromCustomFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName2, textureFilename);
	result = m_Model->ImportFromGLTF(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName1);
	// result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not import the model object.", L"Error", MB_OK);
		return false;
	}

	char* videoCardName = new char[128];
	int memory = 0;

	m_Direct3D->GetVideoCardInfo(videoCardName, memory);

	std::ofstream fout;
	fout.open("video_card_info.txt");
	fout << videoCardName << ' ' << memory << std::endl;
	fout.close();

	m_LightShader = new LightShaderClass;
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_CartoonShader = new CartoonShaderClass;
	result = m_CartoonShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cartoon shader object.", L"Error", MB_OK);
		return false;
	}

	m_DirectionalLight = new LightClass;
	m_DirectionalLight->SetDiffuseColor(1, 1, 1, 1);
	m_DirectionalLight->SetDirection(-1, -1, 1);

	// HLSL array 크기와 NUM_LIGHTS 차이가 날 경우
	// Shader class 에서 알아서 제거하도록 구현해둠
	m_PointLights = new PointLightClass[NUM_LIGHTS];
	m_PointLights[0].SetDiffuseColor(1, 0, 0, 1);
	m_PointLights[0].SetPosition(2, 0, -15);
	m_PointLights[0].SetAttenuationFactors(0, 0, 0.1);

	m_PointLights[1].SetDiffuseColor(0, 0, 1, 1);
	m_PointLights[1].SetPosition(-2, 0, -15);
	m_PointLights[1].SetAttenuationFactors(0, 0, 0.1);

	m_CartoonShaderInput = new CartoonShaderInput;


	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_PointLights)
	{
		delete[] m_PointLights;
		m_PointLights = 0;
	}

	if (m_CartoonShaderInput)
	{
		delete m_CartoonShaderInput;
		m_CartoonShaderInput = 0;
	}

	if (m_DirectionalLight)
	{
		delete m_DirectionalLight;
		m_DirectionalLight = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_CartoonShader)
	{
		m_CartoonShader->Shutdown();
		delete m_CartoonShader;
		m_CartoonShader = 0;
	}

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

	return;
}

bool ApplicationClass::Frame()
{
	static float rotation = 0.0f;
	bool result;

	rotation -= 0.0174532925f * 1.0f;  // radian 단위 갱신
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
	{
		return false;
	}
}

void ApplicationClass::MoveCameraForward(float delta)
{
	XMFLOAT3 rotation = m_Camera->GetRotation();
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x * 0.0174532925f, rotation.y * 0.0174532925f, rotation.z * 0.0174532925f);
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 forwardVector;
	XMStoreFloat3(&forwardVector, rotationMat.r[2]);
	m_Camera->SetPosition(position.x + forwardVector.x * delta, position.y + forwardVector.y * delta, position.z + forwardVector.z * delta);
}

void ApplicationClass::MoveCameraRight(float delta)
{
	XMFLOAT3 rotation = m_Camera->GetRotation();
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x * 0.0174532925f, rotation.y * 0.0174532925f, rotation.z * 0.0174532925f);
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 rightVector;
	XMStoreFloat3(&rightVector, rotationMat.r[0]);
	m_Camera->SetPosition(position.x + rightVector.x * delta, position.y + rightVector.y * delta, position.z + rightVector.z * delta);
}

void ApplicationClass::MoveCameraUp(float delta)
{
	XMFLOAT3 rotation = m_Camera->GetRotation();
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x * 0.0174532925f, rotation.y * 0.0174532925f, rotation.z * 0.0174532925f);
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 rightVector;
	XMStoreFloat3(&rightVector, rotationMat.r[1]);
	m_Camera->SetPosition(position.x + rightVector.x * delta, position.y + rightVector.y * delta, position.z + rightVector.z * delta);
}

bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX scaleMatrix, rotateMatrix, translateMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.0f, 1.0f);  // yellow

	m_Camera->Render();  // view matrix 계산

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// worldMatrix = XMMatrixMultiply(XMMatrixRotationY(rotation), XMMatrixRotationX(rotation / 2));
	
	float scale = 0.07;
	// 행렬이 곱해지는 순서가 SRT 가 되도록 한다.
	// row major 이므로 왼쪽부터 곱해짐

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	// rotateMatrix = XMMatrixRotationY(rotation);
	rotateMatrix = XMMatrixRotationY(0.0174532925f * 160);
	translateMatrix = XMMatrixTranslation(0, -1, -13);
	// 정상 순서 (SRT)
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	// 잘못된 순서 (TSR)
	// 크기 변화 시 이동량이 변화함
	// 축에서 벗어난 곳에서 회전되기 때문에 특정 좌표를 기준으로 한 바퀴를 도는 모습을 보임
	// worldMatrix = XMMatrixMultiply(translateMatrix, XMMatrixMultiply(scaleMatrix, rotateMatrix));

	// Scale 테스트
	// worldMatrix = XMMatrixMultiply(XMMatrixScaling(0.13, 0.07, 0.07), XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_Model->Render(m_Direct3D->GetDeviceContext());

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	// m_CartoonShaderInput->texture = m_Model->GetTexture();
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	// m_CartoonShaderInput->pointLightsNum = NUM_LIGHTS;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;

	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	rotateMatrix = XMMatrixRotationY(0.0174532925f * 160);
	translateMatrix = XMMatrixTranslation(0, -1, -13);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_Model->Render(m_Direct3D->GetDeviceContext());

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	// m_CartoonShaderInput->texture = m_Model->GetTexture();
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	// m_CartoonShaderInput->pointLightsNum = NUM_LIGHTS;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = true;

	m_Direct3D->SetRasterizerFrontCounterClockwise(true);

	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	rotateMatrix = XMMatrixRotationY(0.0174532925f * 160);
	translateMatrix = XMMatrixTranslation(1, -1, -13);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_Model->Render(m_Direct3D->GetDeviceContext());

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	// m_CartoonShaderInput->texture = m_Model->GetTexture();
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	// m_CartoonShaderInput->pointLightsNum = NUM_LIGHTS;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;

	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	translateMatrix = XMMatrixTranslation(1, -1, -5);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	// m_CartoonShaderInput->texture = m_Model->GetTexture();
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	// m_CartoonShaderInput->pointLightsNum = NUM_LIGHTS;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;

	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}
