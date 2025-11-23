 #ifndef _CARTOONSHADERCLASS_H_
#define _CARTOONSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
#include "baseshaderclass.h"
using namespace DirectX;
using namespace std;

struct CartoonShaderInput;

class CartoonShaderClass : public BaseShaderClass
{
private:
	static constexpr int NUM_LIGHTS = 4;

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
	CartoonShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);
	virtual ~CartoonShaderClass();

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	virtual UINT GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const override;
	virtual bool SetShaderParameters(BaseShaderInput*) override;
	virtual void ShutdownShader() override;
	virtual void RenderShader(BaseShaderInput* input, size_t indexCount, size_t instanceNum) override;

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_pointLightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_utilityVariableBuffer;
};



#endif