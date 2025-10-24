#pragma once
#include "bitmapclass.h"

class SpriteClass : public BitmapClass
{
public:
	SpriteClass();
	virtual ~SpriteClass();

	bool ImportFromSpriteFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName);
	
	/*
	*	�ִϸ��̼�ó�� �̹����� ���ŵǵ��� ��
	*/
	bool UpdateImage(ID3D11DeviceContext* deviceContext);

private:
};