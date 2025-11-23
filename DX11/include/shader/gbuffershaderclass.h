#pragma once
#include "baseshaderclass.h"

class GBufferShaderClass : public BaseShaderClass
{
private:
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

public:
	GBufferShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	virtual bool SetShaderParameters(BaseShaderInput*) override;
	virtual void ShutdownShader() override;
	virtual void RenderShader(BaseShaderInput* input, size_t indexCount, size_t instanceNum) override;

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
};