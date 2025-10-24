#include "shader/bitmapshaderclass.h"
#include "shader/baseshaderinput.h"


BitmapShaderClass::BitmapShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename):
	BaseShaderClass(vsFilename, psFilename)
{

}

UINT BitmapShaderClass::GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const
{
	if (!pInputElementDesc) return 0;

	UINT numElements = 5;
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

bool BitmapShaderClass::SetShaderParameters(BaseShaderInput* input)
{
	BaseShaderClass::SetShaderParameters(input);

	input->deviceContext->PSSetShaderResources(0, 1, &input->gltfTextureArrayView);

	return true;
}
