#pragma once
#include "baseshaderclass.h"


class NormalDepthShaderClass : public BaseShaderClass
{
public:
	NormalDepthShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);

private:
	virtual void RenderShader(BaseShaderInput* input, int indexCount) override;
};