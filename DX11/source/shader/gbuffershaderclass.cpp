#include "shader/gbuffershaderclass.h"
#include "shader/baseshaderinput.h"
#include "shader/cartoonshaderinput.h"

GBufferShaderClass::GBufferShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename):
	BaseShaderClass(vsFilename, psFilename)
{
}

bool GBufferShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	BaseShaderClass::InitializeShader(device, hwnd, vsFilename, psFilename);
	HRESULT result;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool GBufferShaderClass::SetShaderParameters(BaseShaderInput* input)
{
	BaseShaderClass::SetShaderParameters(input);
	HRESULT result;
	LightBufferType* lightBufferPtr;
	CameraBufferType* cameraBufferPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CartoonShaderInput* CartoonInput = dynamic_cast<CartoonShaderInput*>(input);

	if (CartoonInput == nullptr)
		return false;

	// Lock the light constant buffer so it can be written to.
	result = CartoonInput->deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	lightBufferPtr = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	lightBufferPtr->diffuseColor = CartoonInput->directionalLight->GetDiffuseColor();
	lightBufferPtr->lightDirection = CartoonInput->directionalLight->GetDirection();
	lightBufferPtr->padding = 0.0f;

	// Unlock the constant buffer.
	CartoonInput->deviceContext->Unmap(m_lightBuffer, 0);

	result = CartoonInput->deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	cameraBufferPtr = (CameraBufferType*)mappedResource.pData;

	cameraBufferPtr->cameraLocation = CartoonInput->cameraLocation;
	cameraBufferPtr->padding = 0.0f;
	CartoonInput->deviceContext->Unmap(m_cameraBuffer, 0);

	// LightBuffer : register(b0)
	CartoonInput->deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	// CameraBuffer : register(b1)
	CartoonInput->deviceContext->PSSetConstantBuffers(1, 1, &m_cameraBuffer);

	return true;
}

void GBufferShaderClass::ShutdownShader()
{
	BaseShaderClass::ShutdownShader();

	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
}

void GBufferShaderClass::RenderShader(BaseShaderInput* input, int indexCount)
{
	// depth SRV 사용을 위해서 RTV 에서 unbind 해줘야함
	input->d3dclass->UnbindDepthStencilView();
	input->deviceContext->PSSetShaderResources(0, 1, input->d3dclass->GetAlbedoSRV());
	input->deviceContext->PSSetShaderResources(1, 1, input->d3dclass->GetNormalSRV());
	input->deviceContext->PSSetShaderResources(2, 1, input->d3dclass->GetPositionSRV());
	input->deviceContext->PSSetShaderResources(3, 1, input->d3dclass->GetDepthStencilSRV());
	BaseShaderClass::RenderShader(input, indexCount);
}
