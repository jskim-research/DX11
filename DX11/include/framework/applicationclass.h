/*
그래픽 관련 클래스
*/

#pragma once
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

//////////////
// INCLUDES //
//////////////
#include "d3dclass.h"
#include "camera/cameraclass.h"
#include "model/modelclass.h"
#include "shader/textureshaderclass.h"
#include "shader/lightshaderclass.h"
#include "light/lightclass.h"
#include "light/pointlightclass.h"
#include "shader/cartoonshaderclass.h"
#include "shader/cartoonshaderinput.h"
#include "model/bitmapclass.h"
#include "shader/baseshaderclass.h"
#include "shader/baseshaderinput.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;
const int NUM_LIGHTS = 2;


#endif
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	// view matrix 이후엔 z 축이 forward, x 축이 right, y 축이 up 임
	void MoveCameraForward(float delta);
	void MoveCameraRight(float delta);
	void MoveCameraUp(float delta);
	void AddCameraRotation(float x, float y, float z);

private:
	bool Render(float);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	CartoonShaderClass* m_CartoonShader;
	CartoonShaderInput* m_CartoonShaderInput;
	LightClass* m_DirectionalLight;
	PointLightClass* m_PointLights;
	BitmapClass* m_Bitmap;
	BaseShaderClass* m_BaseShader;
	BaseShaderInput* m_BaseShaderInput;

};