#pragma once

#include "modelclass.h"

/*
*	left, bottom => (- screen width / 2, - screen height / 2)
*	right, top => (screen width / 2, screen height / 2)
*	(0, 0) => È­¸é Áß¾Ó
*/
class BitmapClass : public ModelClass
{
public:
	BitmapClass();
	virtual ~BitmapClass();

	virtual void Initialize(D3DClass* direct3D) override;

	virtual bool UpdateRenderPosition(ID3D11DeviceContext* deviceContext, int renderPositionX, int renderPositionY);

	void SetWidth(int width);
	void SetHeight(int height);

protected:
	int m_renderPositionX;
	int m_renderPositionY;
	int m_prevRenderPositionX;
	int m_prevRenderPositionY;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
};