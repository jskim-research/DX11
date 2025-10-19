#include "shader/cartoonshaderclass.h"
#include "shader/cartoonshaderinput.h"

CartoonShaderClass::CartoonShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename):
    BaseShaderClass(vsFilename, psFilename),
    m_lightBuffer(0),
    m_pointLightBuffer(0),
    m_cameraBuffer(0),
    m_utilityVariableBuffer(0)
    
{
}

CartoonShaderClass::~CartoonShaderClass()
{
}

bool CartoonShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFilename, WCHAR * psFilename)
{
    BaseShaderClass::InitializeShader(device, hwnd, vsFilename, psFilename);

    HRESULT result;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    D3D11_BUFFER_DESC pointLightBufferDesc;
    D3D11_BUFFER_DESC cameraBufferDesc;
    D3D11_BUFFER_DESC utilityVariableBufferDesc;

    // Setup the description of the light dynamic constant buffer that is in the pixel shader.
    // Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
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

    pointLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    pointLightBufferDesc.ByteWidth = sizeof(PointLightBuffer);
    pointLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pointLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pointLightBufferDesc.MiscFlags = 0;
    pointLightBufferDesc.StructureByteStride = 0;
    result = device->CreateBuffer(&pointLightBufferDesc, NULL, &m_pointLightBuffer);
    if (FAILED(result))
    {
        return false;
    }

    utilityVariableBufferDesc.ByteWidth = sizeof(UtilityVariableBufferType);
    utilityVariableBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    utilityVariableBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    utilityVariableBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    utilityVariableBufferDesc.MiscFlags = 0;
    utilityVariableBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&utilityVariableBufferDesc, 0, &m_utilityVariableBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

UINT CartoonShaderClass::GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const
{
    if (!pInputElementDesc) return 0;

    UINT numElements = 5;
    *pInputElementDesc = new D3D11_INPUT_ELEMENT_DESC[numElements];

    // Create the vertex input layout description.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

    (*pInputElementDesc)[3].SemanticName = "COLOR";
    (*pInputElementDesc)[3].SemanticIndex = 0;
    (*pInputElementDesc)[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    (*pInputElementDesc)[3].InputSlot = 0;
    (*pInputElementDesc)[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    (*pInputElementDesc)[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    (*pInputElementDesc)[3].InstanceDataStepRate = 0;

    (*pInputElementDesc)[4].SemanticName = "TEXCOORD";
    (*pInputElementDesc)[4].SemanticIndex = 1;
    (*pInputElementDesc)[4].Format = DXGI_FORMAT_R32_UINT;
    (*pInputElementDesc)[4].InputSlot = 0;
    (*pInputElementDesc)[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    (*pInputElementDesc)[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    (*pInputElementDesc)[4].InstanceDataStepRate = 0;

    return numElements;
}

bool CartoonShaderClass::SetShaderParameters(BaseShaderInput* Input)
{
    BaseShaderClass::SetShaderParameters(Input);

    CartoonShaderInput* CartoonInput = dynamic_cast<CartoonShaderInput*>(Input);

    if (CartoonInput == nullptr)
        return false;

    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    LightBufferType* lightBufferPtr;
    CameraBufferType* cameraBufferPtr;
    PointLightBuffer* pointLightPtr;
    UtilityVariableBufferType* utilityVariableBufferPtr;

    CartoonInput->deviceContext->PSSetShaderResources(1, 1, &CartoonInput->gltfTextureArrayView);

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

    result = CartoonInput->deviceContext->Map(m_pointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    pointLightPtr = (PointLightBuffer*)mappedResource.pData;
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (i >= CartoonInput->pointLightsNum)
        {
            pointLightPtr->lights[i].diffuseColor = XMFLOAT4(0, 0, 0, 0);
            pointLightPtr->lights[i].lightPosition = XMFLOAT3(0, 0, 0);
            pointLightPtr->lights[i].attenuationFactors = XMFLOAT3(1, 0, 0);
            pointLightPtr->lights[i].padding1 = 0.0f;
            pointLightPtr->lights[i].padding2 = 0.0f;
        }
        else
        {
            pointLightPtr->lights[i].diffuseColor = CartoonInput->pointLights[i].GetDiffuseColor();
            pointLightPtr->lights[i].lightPosition = CartoonInput->pointLights[i].GetPosition();
            pointLightPtr->lights[i].attenuationFactors = CartoonInput->pointLights[i].GetAttenuationFactors();
            pointLightPtr->lights[i].padding1 = 0.0f;
            pointLightPtr->lights[i].padding2 = 0.0f;
        }
    }
    CartoonInput->deviceContext->Unmap(m_pointLightBuffer, 0);

    result = CartoonInput->deviceContext->Map(m_utilityVariableBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    utilityVariableBufferPtr = (UtilityVariableBufferType*)mappedResource.pData;
    utilityVariableBufferPtr->isOutline = CartoonInput->isOutline;
    utilityVariableBufferPtr->padding = XMFLOAT3(0, 0, 0);

    CartoonInput->deviceContext->Unmap(m_utilityVariableBuffer, 0);

    // LightBuffer : register(b0)
    CartoonInput->deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);

    // CameraBuffer : register(b1)
    CartoonInput->deviceContext->PSSetConstantBuffers(1, 1, &m_cameraBuffer);

    // PointLightBuffer : register(b2)
    CartoonInput->deviceContext->PSSetConstantBuffers(2, 1, &m_pointLightBuffer);

    // UtilityVariableBufferType : register(b3)
    CartoonInput->deviceContext->PSSetConstantBuffers(3, 1, &m_utilityVariableBuffer);

    // UtilityVariableBufferType : register(b1)
    CartoonInput->deviceContext->VSSetConstantBuffers(1, 1, &m_utilityVariableBuffer);

    // CameraBuffer : register(b2)
    CartoonInput->deviceContext->VSSetConstantBuffers(2, 1, &m_cameraBuffer);

    return true;
}

void CartoonShaderClass::ShutdownShader()
{
    BaseShaderClass::ShutdownShader();

    if (m_utilityVariableBuffer)
    {
        m_utilityVariableBuffer->Release();
        m_utilityVariableBuffer = 0;
    }

    if (m_pointLightBuffer)
    {
        m_pointLightBuffer->Release();
        m_pointLightBuffer = 0;
    }

    // Release the light constant buffer.
    if (m_lightBuffer)
    {
        m_lightBuffer->Release();
        m_lightBuffer = 0;
    }

    if (m_cameraBuffer)
    {
        m_cameraBuffer->Release();
        m_cameraBuffer = 0;
    }
}
