#pragma once
#include "baseshaderclass.h"

class GBufferShaderClass : public BaseShaderClass
{
public:
	GBufferShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);

private:
	virtual void RenderShader(BaseShaderInput* input, int indexCount) override;
};