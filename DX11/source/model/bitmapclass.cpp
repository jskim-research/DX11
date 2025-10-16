#include "model/bitmapclass.h"
#include "framework/d3dclass.h"

BitmapClass::BitmapClass() :
	m_renderPositionX(0),
	m_renderPositionY(0),
	m_prevRenderPositionX(-1),
	m_prevRenderPositionY(-1),
	m_bitmapWidth(200),
	m_bitmapHeight(200),
	m_screenWidth(0),
	m_screenHeight(0)
{
	m_vertexBufferUsage = D3D11_USAGE_DYNAMIC;
	m_vertexBufferCPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// index buffer 는 안 건들임
	m_indexBufferUsage = D3D11_USAGE_DEFAULT;
	m_indexBufferCPUAccessFlags = 0;

}

BitmapClass::~BitmapClass()
{
}

void BitmapClass::Initialize(D3DClass* direct3D)
{
	ModelClass::Initialize(direct3D);

	m_screenWidth = direct3D->GetScreenWidth();
	m_screenHeight = direct3D->GetScreenHeight();
}

bool BitmapClass::UpdateRenderPosition(ID3D11DeviceContext* deviceContext, int renderPositionX, int renderPositionY)
{
	if (m_prevRenderPositionX == renderPositionX && m_prevRenderPositionY == renderPositionY)
	{
		return false;
	}

	m_prevRenderPositionX = m_renderPositionX;
	m_prevRenderPositionY = m_renderPositionY;
	m_renderPositionX = renderPositionX;
	m_renderPositionY = renderPositionY;

	// 새로운 render position 에 따라 vertex buffer, index buffer 갱신
	ObjectParser::CommonVertexType* vertices = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;

	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	vertices = (ObjectParser::CommonVertexType*)mappedResource.pData;

	int left = m_renderPositionX - (m_bitmapWidth / 2);
	int right = m_renderPositionX + (m_bitmapWidth / 2);
	int top = m_renderPositionY + (m_bitmapHeight / 2);
	int bottom = m_renderPositionY - (m_bitmapHeight / 2);

	vertices[0].position = XMFLOAT3(left, top, vertices[0].position.z);
	vertices[1].position = XMFLOAT3(right, top, vertices[1].position.z);
	vertices[2].position = XMFLOAT3(left, bottom, vertices[2].position.z);
	vertices[3].position = XMFLOAT3(left, bottom, vertices[3].position.z);
	vertices[4].position = XMFLOAT3(right, top, vertices[4].position.z);
	vertices[5].position = XMFLOAT3(right, bottom, vertices[5].position.z);

	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}
