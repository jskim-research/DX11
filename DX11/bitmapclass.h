#pragma once

#include "modelclass.h"

/*
*	left, bottom => (-, -)
*	right, top => (+, +)
*	¿øÁ¡ => (0, 0)
*/
class BitmapClass : public ModelClass
{
public:
	BitmapClass();
	virtual ~BitmapClass();

	virtual void Initialize(D3DClass* direct3D) override;

	bool UpdateRenderPosition(ID3D11DeviceContext* deviceContext, int renderPositionX, int renderPositionY);

private:
	int m_renderPositionX;
	int m_renderPositionY;
	int m_prevRenderPositionX;
	int m_prevRenderPositionY;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
};