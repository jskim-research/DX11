#pragma once
#include "baseshaderclass.h"

class BitmapShaderClass : public BaseShaderClass
{
public:
	BitmapShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);

protected:
	virtual UINT GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const override;
	// �ڽ� Ŭ�������� BaseShaderInput �� dynamic_cast ���� ���� ĳ�����ؼ� ����ؾ���
	virtual bool SetShaderParameters(BaseShaderInput* input) override;
};