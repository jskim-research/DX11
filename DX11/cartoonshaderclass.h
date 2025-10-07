#ifndef _CARTOONSHADERCLASS_H_
#define _CARTOONSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
using namespace DirectX;
using namespace std;

struct CartoonShaderInput;

class CartoonShaderClass
{
private:
	static constexpr int NUM_LIGHTS = 4;
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraLocation;
		float padding;
	};

	struct PointLightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightPosition;
		float padding1;
		XMFLOAT3 attenuationFactors;
		float padding2;
	};

	struct PointLightBuffer
	{
		PointLightBufferType lights[NUM_LIGHTS];
	};

	struct UtilityVariableBufferType
	{
		BOOL isOutline;
		XMFLOAT3 padding;
	};

public:
	CartoonShaderClass();
	CartoonShaderClass(const CartoonShaderClass&);
	~CartoonShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(CartoonShaderInput*, int);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(CartoonShaderInput*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_pointLightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_utilityVariableBuffer;
};



#endif