#include "framework/applicationclass.h"
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
	m_Bitmap = 0;
	m_BaseShader = 0;
	m_BaseShaderInput = 0;
	m_BitmapShader = 0;
	m_BitmapShaderInput = 0;
	m_Sprite = 0;
	m_NormalDepthShader = 0;
	m_Cube = 0;
	m_GBufferShader = 0;
	m_FullScreenTriangle = 0;
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
	char objFileName[128];
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
	strcpy_s(objFileName, "./data/burnice.glb");
	result = m_Model->ImportFromGLTF(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName);
	if (!result)
	{
		MessageBox(hwnd, L"Could not import the model object.", L"Error", MB_OK);
		return false;
	}

	m_Cube = new ModelClass;
	strcpy_s(objFileName, "./data/Cube.txt");
	strcpy_s(textureFilename, "./data/stone01.tga");
	result = m_Cube->ImportFromCustomFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not import the cube object.", L"Error", MB_OK);
		return false;
	}

	char* videoCardName = new char[128];
	int memory = 0;

	m_Direct3D->GetVideoCardInfo(videoCardName, memory);

	std::ofstream fout;
	fout.open("./log/video_card_info.txt");
	fout << videoCardName << ' ' << memory << std::endl;
	fout.close();

	m_LightShader = new LightShaderClass;
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_CartoonShader = new CartoonShaderClass(L"./hlsl/cartoon.vs", L"./hlsl/cartoon.ps");
	result = m_CartoonShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cartoon shader object.", L"Error", MB_OK);
		return false;
	}

	m_BaseShader = new BaseShaderClass(L"./hlsl/base.vs", L"./hlsl/base.ps");
	result = m_BaseShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the base shader object.", L"Error", MB_OK);
		return false;
	}

	m_BitmapShader = new BitmapShaderClass(L"./hlsl/bitmap.vs", L"./hlsl/bitmap.ps");
	result = m_BitmapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap shader object.", L"Error", MB_OK);
		return false;
	}

	m_NormalDepthShader = new NormalDepthShaderClass(L"./hlsl/normaldepth.vs", L"./hlsl/normaldepth.ps");
	result = m_NormalDepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the normal depth shader object.", L"Error", MB_OK);
		return false;
	}

	m_GBufferShader = new GBufferShaderClass(L"./hlsl/gbuffer_combine.vs", L"./hlsl/gbuffer_combine.ps");
	result = m_GBufferShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the gbuffer shader object.", L"Error", MB_OK);
		return false;
	}

	m_Bitmap = new BitmapClass;
	strcpy_s(objFileName, "./data/Bitmap.txt");
	strcpy_s(textureFilename, "./data/stone01.tga");
	result = m_Bitmap->ImportFromCustomFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not import the bitmap object.", L"Error", MB_OK);
		return false;
	}

	m_Sprite = new SpriteClass;
	strcpy_s(objFileName, "./data/Bitmap.txt");
	strcpy_s(textureFilename, "./data/sprite_data_01.txt");
	result = m_Sprite->ImportFromSpriteFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not import the sprite object.", L"Error", MB_OK);
		return false;
	}

	m_FullScreenTriangle = new ModelClass;
	strcpy_s(objFileName, "./data/FullScreenTriangle.txt");
	// strcpy_s(textureFilename, "./data/sprite_data_01.txt");
	result = m_FullScreenTriangle->ImportFromCustomFile(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), objFileName, nullptr);
	if (!result)
	{
		MessageBox(hwnd, L"Could not import the full screen triangle object.", L"Error", MB_OK);
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
	m_BaseShaderInput = new BaseShaderInput;
	m_BitmapShaderInput = new BitmapShaderInput;


	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_FullScreenTriangle)
	{
		m_FullScreenTriangle->Shutdown();
		delete m_FullScreenTriangle;
		m_FullScreenTriangle = 0;
	}

	if (m_GBufferShader)
	{
		m_GBufferShader->Shutdown();
		delete m_GBufferShader;
		m_GBufferShader = 0;
	}
	if (m_Cube)
	{
		m_Cube->Shutdown();
		delete m_Cube;
		m_Cube = 0;
	}

	if (m_NormalDepthShader)
	{
		m_NormalDepthShader->Shutdown();
		delete m_NormalDepthShader;
		m_NormalDepthShader = 0;
	}

	if (m_Sprite)
	{
		m_Sprite->Shutdown();
		delete m_Sprite;
		m_Sprite = 0;
	}

	if (m_BitmapShaderInput)
	{
		delete m_BitmapShaderInput;
		m_BitmapShaderInput = 0;
	}

	if (m_BitmapShader)
	{
		m_BitmapShader->Shutdown();
		delete m_BitmapShader;
		m_BitmapShader = 0;
	}

	if (m_BaseShaderInput)
	{
		delete m_BaseShaderInput;
		m_BaseShaderInput = 0;
	}
	if (m_BaseShader)
	{
		m_BaseShader->Shutdown();
		delete m_BaseShader;
		m_BaseShader = 0;
	}

	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

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

void ApplicationClass::AddCameraRotation(float x, float y, float z)
{
	m_Camera->AddRotation(x, y, z);
}

bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX scaleMatrix, rotateMatrix, translateMatrix;
	bool result;

	// Clear the render target view & depth stencil view to begin the scene.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.0f, 1.0f);  // yellow background

	/************************************************/
	/*                 Drawing Mesh                 */
	/************************************************/
	
	// m_CartoonShader draws on GBuffer (albedo, normal, depth)

	m_Camera->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	float scale = 0.07;
	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	rotateMatrix = XMMatrixRotationY(0.0174532925f * 160);
	translateMatrix = XMMatrixTranslation(0, -1, -13);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_CartoonShaderInput->d3dclass = m_Direct3D;
	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = true;
	m_Direct3D->SetRasterizerFrontCounterClockwise(true);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	// outline 용으로 활성화했던 것 다시 초기화
	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	translateMatrix = XMMatrixTranslation(0, -1, -13);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;
	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	rotateMatrix = XMMatrixRotationY(0.0174532925f * 160);
	translateMatrix = XMMatrixTranslation(1, -1, -13);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	m_CartoonShaderInput->gltfTextureArrayView = m_Model->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;
	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Model->GetIndexCount());
	if (!result)
	{
		return false;
	}

	scaleMatrix = XMMatrixScaling(scale, scale, scale);
	rotateMatrix = XMMatrixRotationY(0.0174532925f * 160);
	translateMatrix = XMMatrixTranslation(-1, 0, -13);
	worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixMultiply(rotateMatrix, translateMatrix));

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	m_CartoonShaderInput->gltfTextureArrayView = m_Cube->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;
	m_Direct3D->SetRasterizerFrontCounterClockwise(false);

	m_Cube->Render(m_Direct3D->GetDeviceContext());
	result = m_CartoonShader->Render(m_CartoonShaderInput, m_Cube->GetIndexCount());
	if (!result)
	{
		return false;
	}

	/***************************************************/
	/*                 GBuffer Combine                 */
	/***************************************************/

	/*
	*	G-Buffer 모두 합쳐서 그리는 Shader
	*	참고로 모든 pixel 에 overwrite 하기 때문에 이전에 그린 것들 전부 사라진다.
	*/

	m_CartoonShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_CartoonShaderInput->worldMatrix = worldMatrix;
	m_CartoonShaderInput->viewMatrix = viewMatrix;
	m_CartoonShaderInput->projectionMatrix = projectionMatrix;
	m_CartoonShaderInput->gltfTextureArrayView = m_FullScreenTriangle->GetGltfTextures();
	m_CartoonShaderInput->cameraLocation = m_Camera->GetPosition();
	m_CartoonShaderInput->directionalLight = m_DirectionalLight;
	m_CartoonShaderInput->pointLights = m_PointLights;
	m_CartoonShaderInput->pointLightsNum = 0;
	m_CartoonShaderInput->isOutline = false;

	m_FullScreenTriangle->Render(m_Direct3D->GetDeviceContext());
	result = m_GBufferShader->Render(m_CartoonShaderInput, m_FullScreenTriangle->GetIndexCount());
	if (!result)
	{
		return false;
	}

	/*******************************************/
	/*                 Draw UI                 */
	/*******************************************/

	/*
	// Bitmap 렌더링
	// 1280 x 720
	m_Bitmap->UpdateRenderPosition(m_Direct3D->GetDeviceContext(), -640 + 150, 360 - 150);
	m_Bitmap->Render(m_Direct3D->GetDeviceContext());
	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();
	m_Direct3D->GetOrthoMatrix(projectionMatrix);

	m_BitmapShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_BitmapShaderInput->worldMatrix = worldMatrix;
	m_BitmapShaderInput->viewMatrix = viewMatrix;
	m_BitmapShaderInput->projectionMatrix = projectionMatrix;
	m_BitmapShaderInput->gltfTextureArrayView = m_Bitmap->GetGltfTextures();
	m_BitmapShaderInput->d3dclass = m_Direct3D;
	// 이걸 꺼야 depth test 무시하고 screen 에 바로 뜸
	m_Direct3D->SetZBufferOnOff(false);

	result = m_BitmapShader->Render(m_BitmapShaderInput, m_Bitmap->GetIndexCount());
	if (!result)
	{
		return false;
	}

	m_Direct3D->SetZBufferOnOff(true);

	// Sprite 렌더링
	// Sprite 가 Bitmap 을 상속하므로 Input 등은 그대로 사용하였음
	// 1280 x 720
	m_Sprite->UpdateRenderPosition(m_Direct3D->GetDeviceContext(), -640 + 400, 360 - 150);
	m_Sprite->Render(m_Direct3D->GetDeviceContext());
	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();
	m_Direct3D->GetOrthoMatrix(projectionMatrix);

	m_BitmapShaderInput->deviceContext = m_Direct3D->GetDeviceContext();
	m_BitmapShaderInput->worldMatrix = worldMatrix;
	m_BitmapShaderInput->viewMatrix = viewMatrix;
	m_BitmapShaderInput->projectionMatrix = projectionMatrix;
	m_BitmapShaderInput->gltfTextureArrayView = m_Sprite->GetGltfTextures();
	// 이걸 꺼야 depth test 무시하고 screen 에 바로 뜸
	m_Direct3D->SetZBufferOnOff(false);

	result = m_BitmapShader->Render(m_BitmapShaderInput, m_Sprite->GetIndexCount());
	if (!result)
	{
		return false;
	}

	m_Direct3D->SetZBufferOnOff(true);
	*/
	
	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}
