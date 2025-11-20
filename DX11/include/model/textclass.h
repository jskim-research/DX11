#pragma once
#include "bitmapclass.h"
#include "font/fontclass.h"


class TextClass : public BitmapClass
{
public:
	TextClass();
	virtual ~TextClass();

	bool BuildBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* str, FontClass& font, XMFLOAT4 color);
	virtual bool UpdateRenderPosition(ID3D11DeviceContext* deviceContext, int renderPositionX, int renderPositionY) override;

protected:
	virtual void ShutdownModel() override;

private:
	char* m_Str;
	size_t m_StrLength;
	FontClass* m_Font;
};