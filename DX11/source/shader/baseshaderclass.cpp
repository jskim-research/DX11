#include "shader/baseshaderclass.h"
#include "shader/baseshaderinput.h"

BaseShaderClass::BaseShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename):
	m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_samplerState(0),
	m_matrixBuffer(0),
	m_vsFilename(vsFilename),
	m_psFilename(psFilename)
{
}

BaseShaderClass::~BaseShaderClass()
{
	// Shutdown 을 외부에서 잊지않고 해줬기를 바람
	// virtual 이 포함돼서 소멸자에 넣을 수 없음
}

bool BaseShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	errno_t error;
	WCHAR vsFilename[128];
	WCHAR psFilename[128];
	
	error = wcscpy_s(vsFilename, 128, m_vsFilename);
	if (error != 0)
		return false;

	error = wcscpy_s(psFilename, 128, m_psFilename);
	if (error != 0)
		return false;

	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
		return false;

	return true;
}

bool BaseShaderClass::Render(BaseShaderInput* input, int indexCount)
{
	bool result;

	result = SetShaderParameters(input);
	if (!result)
		return false;

	RenderShader(input->deviceContext, indexCount);
	
	return true;
}

void BaseShaderClass::Shutdown()
{
	ShutdownShader();
}

bool BaseShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc;  // vertex shader 랑 연결
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, nullptr, nullptr, "VSMain", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		return false;
	}

	result = D3DCompileFromFile(psFilename, nullptr, nullptr, "PSMain", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(result))
		return false;

	unsigned int numElements = GetInputElementDesc(&inputElementDesc);
	result = device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 0~1 로 wrap
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result))
		return false;

	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	return true;
}

UINT BaseShaderClass::GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const
{
	if (!pInputElementDesc) return 0;

	UINT numElements = 3;
	*pInputElementDesc = new D3D11_INPUT_ELEMENT_DESC[numElements];

	(*pInputElementDesc)[0].SemanticName = "POSITION";
	(*pInputElementDesc)[0].SemanticIndex = 0;
	(*pInputElementDesc)[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	(*pInputElementDesc)[0].InputSlot = 0;
	(*pInputElementDesc)[0].AlignedByteOffset = 0;
	(*pInputElementDesc)[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	(*pInputElementDesc)[0].InstanceDataStepRate = 0;

	(*pInputElementDesc)[1].SemanticName = "NORMAL";
	(*pInputElementDesc)[1].SemanticIndex = 0;
	(*pInputElementDesc)[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	(*pInputElementDesc)[1].InputSlot = 0;
	(*pInputElementDesc)[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	(*pInputElementDesc)[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	(*pInputElementDesc)[1].InstanceDataStepRate = 0;

	(*pInputElementDesc)[2].SemanticName = "TEXCOORD";
	(*pInputElementDesc)[2].SemanticIndex = 0;
	(*pInputElementDesc)[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	(*pInputElementDesc)[2].InputSlot = 0;
	(*pInputElementDesc)[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	(*pInputElementDesc)[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	(*pInputElementDesc)[2].InstanceDataStepRate = 0;
	return numElements;
}

bool BaseShaderClass::SetShaderParameters(BaseShaderInput* input)
{
	HRESULT result;
	MatrixBuffer* pMatrixData;
	D3D11_MAPPED_SUBRESOURCE matrixData;

	result = input->deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matrixData);
	if (FAILED(result))
		return false;
	pMatrixData = (MatrixBuffer*)matrixData.pData;
	// XMMATRIX 는 row1, row2, row3, row4 4개의 XMVECTOR 로 구성되는데
	// HLSL matrix 는 기본적으로 column-major 로 받으므로 col1, col2, col3, col4 로 해석해버림
	// row vector matrix 를 유지하기 위해선 transpose 를 해서 보내야함
	pMatrixData->world = XMMatrixTranspose(input->worldMatrix);
	pMatrixData->view = XMMatrixTranspose(input->viewMatrix);
	pMatrixData->projection = XMMatrixTranspose(input->projectionMatrix);
	input->deviceContext->Unmap(m_matrixBuffer, 0);

	// MatrixBuffer : register(b0) 이어야함
	input->deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	return true;
}

void BaseShaderClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void BaseShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void BaseShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	if (errorMessage)
	{
		char* compileErrors;
		unsigned __int64 bufferSize, i;
		ofstream fout;


		// Get a pointer to the error message text buffer.
		compileErrors = (char*)(errorMessage->GetBufferPointer());

		// Get the length of the message.
		bufferSize = errorMessage->GetBufferSize();

		// Open a file to write the error message to.
		fout.open("./log/shader-error.txt");

		// Write out the error message.
		for (i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}

		// Close the file.
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = 0;

		// Pop a message up on the screen to notify the user to check the text file for compile errors.
		MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
	}
	else
	{
		MessageBox(hwnd, shaderFilename, L"Missing Shader File", MB_OK);
	}
}