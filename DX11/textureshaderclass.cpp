#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
    m_sampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    wchar_t vsFileName[128];
    wchar_t psFileName[128];
    int error;

    error = wcscpy_s(vsFileName, 128, L"./texture.vs");
    if (error != 0)
    {
        return false;
    }

    error = wcscpy_s(psFileName, 128, L"./texture.ps");
    if (error != 0)
    {
        return false;
    }

    result = InitializeShader(device, hwnd, vsFileName, psFileName);
    if (!result)
    {
        return false;
    }

    return true;
}

void TextureShaderClass::Shutdown()
{
    ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    bool result;

    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);

    if (!result)
    {
        return false;
    }

    RenderShader(deviceContext, indexCount);

    return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
    // vs, ps 에서 필요한 변수들 device 에서 생성
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;

    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    // .vs => TextureVertexShader function 을 entry point 로 설정하고 컴파일
    // vertexShaderBuffer 에 주소 저장 (무엇의 주소인지?)
    result = D3DCompileFromFile(vsFileName, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
        }
        else
        {
            MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
        }
    }

    result = D3DCompileFromFile(psFileName, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
        }
        else
        {
            MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
        }
    }

    // device 에서 생성해준 m_vertexShader 주소 저장
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(result))
    {
        return false;
    }

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
    {
        return false;
    }

    // input layout (즉, VertexInputType) 명시
    // 2개의 변수 있었으므로 각각 순서대로 설정
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD0";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

    // m_vertexShader, m_pixelShader, m_layout 설정에 이미 쓴 vertexShaderBuffer, pixelShaderBuffer 제거
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // world, view, projection matrix 자원 생성
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // sampler state 자원 생성
    // Filter 설정 의미 (멀리서 볼 때, 가까이서 볼 때 등 sample 방식 설정)
    // sampler to use linear interpolation for minification, magnification, and mip-level sampling.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    // WRAP => 0.0f, 1.0f 범위를 벗어나는 값은 0.0f 또는 1.0f 으로 변경됨
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
    {
        return false;
    }
    
    return true;
}

void TextureShaderClass::ShutdownShader()
{
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }

    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;

    compileErrors = (char*)(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();
    fout.open("shader-error.txt");
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    fout.close();

    errorMessage->Release();
    errorMessage = 0;

    MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    // 애초에 matrix 구성할 때 transpose 해서 넣으면 되지 왜 여기서 transpose 하는지?
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    dataPtr = (MatrixBufferType*)mappedResource.pData;

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;
    deviceContext->Unmap(m_matrixBuffer, 0);

    // vertex shader 에서 matrix buffer 가 첫 번째로 나옴 가정 
    bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // 왜 sample state 는 RenderShader 함수로 뺏지?
    // 그리고 register(t0) 는 PSSetShaderResources 이고 register(s0) 는 PSSetSamplers 같은건가?
    // 그리고 texture 나 sample state 는 한번 설정해두면 바뀌지 않는데 render 시마다 넣어주는건 일반적인 flow 를 고려해서 그런가?
    // 아니면 안해두면 자동으로 메모리에서 최적화를 위해 내려가있는 상태일수도 있다든지
    deviceContext->PSSetShaderResources(0, 1, &texture);
    return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_layout);

    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}
