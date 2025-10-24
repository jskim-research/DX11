#pragma once
#include "bitmapclass.h"

class SpriteClass : public BitmapClass
{
public:
	SpriteClass();
	virtual ~SpriteClass();

	bool ImportFromSpriteFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName);
	
	/*
	*	애니메이션처럼 이미지가 갱신되도록 함
	*/
	bool UpdateImage(ID3D11DeviceContext* deviceContext);

private:
};