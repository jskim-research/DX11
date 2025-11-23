#pragma once
#include "baseshaderclass.h"

class TextShaderClass : public BaseShaderClass
{
public:
	TextShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);

protected:
	virtual UINT GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const override;
	// 자식 클래스에서 BaseShaderInput 을 dynamic_cast 등을 통해 캐스팅해서 사용해야함
	virtual bool SetShaderParameters(BaseShaderInput* input) override;
	virtual void RenderShader(BaseShaderInput* input, size_t indexCount, size_t instanceNum) override;
};