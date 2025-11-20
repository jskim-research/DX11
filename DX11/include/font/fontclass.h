#pragma once
#include <d3d11.h>
#include "texture/textureclass.h"

struct CharType
{
	// uv 에서 full height 를 사용하므로, width 만 구분 하는 u1, u2 만 있으면 됨
	double u1;
	double u2;
	int size;
};

/*
*	Char - Texture Mapping 정보 보관 클래스
*/
class FontClass
{
public:	
	FontClass();
	~FontClass();

	void ReleaseData();

	bool ImportFromFontFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName);

	bool GetCharData(char ch, CharType* charInfo) const;
	ID3D11ShaderResourceView* GetTexture() const;

private:
	CharType* m_Chars;
	TextureClass* m_fontTexture;
};