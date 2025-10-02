#ifndef _CARTOONSHADERINPUT_H_
#define _CARTOONSHADERINPUT_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "lightclass.h"

using namespace DirectX;
using namespace std;

struct CartoonShaderInput
{
	ID3D11DeviceContext* deviceContext;
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix; 
	XMMATRIX projectionMatrix;
	ID3D11ShaderResourceView* texture; 
	LightClass* directionalLight;
	ID3D11ShaderResourceView* gltfTextureArrayView;
	XMFLOAT3 cameraLocation;
};


#endif
