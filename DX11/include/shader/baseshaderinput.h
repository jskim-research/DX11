#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "framework/d3dclass.h"

using namespace DirectX;
using namespace std;

struct BaseShaderInput
{
	ID3D11DeviceContext* deviceContext;
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	ID3D11ShaderResourceView* gltfTextureArrayView;
	D3DClass* d3dclass;

	// 다형성 있음을 알림
	virtual ~BaseShaderInput() {}
};