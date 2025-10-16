#ifndef _CARTOONSHADERINPUT_H_
#define _CARTOONSHADERINPUT_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "baseshaderinput.h"
#include "lightclass.h"
#include "pointlightclass.h"

using namespace DirectX;
using namespace std;

struct CartoonShaderInput : public BaseShaderInput
{
	ID3D11ShaderResourceView* texture;
	XMFLOAT3 cameraLocation;
	LightClass* directionalLight;
	PointLightClass* pointLights;
	int pointLightsNum;
	bool isOutline;
};


#endif
