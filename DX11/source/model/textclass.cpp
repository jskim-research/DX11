#include "model/textclass.h"

TextClass::TextClass():
	m_Str(nullptr),
	m_StrLength(0)
{
}

TextClass::~TextClass()
{
}

bool TextClass::BuildBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* str, FontClass& font, XMFLOAT4 color)
{
	bool result = false;
	size_t length = strlen(str);
	ObjectParser::CommonVertexType* vertices = nullptr;
	unsigned long* indices = nullptr;

	m_vertexCount = length * 6;
	m_indexCount = m_vertexCount;

	vertices = new ObjectParser::CommonVertexType[m_vertexCount];
	indices = new unsigned long[m_indexCount];

	if (m_Str != nullptr) delete[] m_Str;
	m_Str = new char[length];
	m_StrLength = length;

	m_Font = &font;
	m_textureArrayView = font.GetTexture();

	for (int i = 0; i < length; i++)
	{
		CharType ct;
		m_Str[i] = str[i];
		result = font.GetCharData(m_Str[i], &ct);
		if (!result) return false;

		// Char 하나당 vertex 6개 (2개의 triangle)
		// UpdateRenderPosition 함수 내에서 처리하는 순서랑 맞추기
		// triangle 1 : lt -> rt -> lb
		// triangle 2 : lb -> rt -> rb
		vertices[i * 6 + 0].texture = XMFLOAT2(ct.u1, 0);
		vertices[i * 6 + 1].texture = XMFLOAT2(ct.u2, 0);
		vertices[i * 6 + 2].texture = XMFLOAT2(ct.u1, 0.5);
		vertices[i * 6 + 3].texture = XMFLOAT2(ct.u1, 0.5);
		vertices[i * 6 + 4].texture = XMFLOAT2(ct.u2, 0);
		vertices[i * 6 + 5].texture = XMFLOAT2(ct.u2, 0.5);

		for (int j = 0; j < 6; j++)
		{
			// 밑처럼 기본값 넣어놔야 culling 안 당함
			vertices[i * 6 + j].position = XMFLOAT3(0, 0, 1);
			vertices[i * 6 + j].normal = XMFLOAT3(0, 0, -1);
			vertices[i * 6 + j].color = color;

			// index 는 vertex index 랑 같음
			indices[i * 6 + j] = i * 6 + j;
		}
	}

	result = MakeVertexBuffer(device, vertices);
	if (!result) return false;

	result = MakeIndexBuffer(device, indices);
	if (!result) return false;

	if (vertices) delete[] vertices;
	if (indices) delete[] indices;

	return true;
}

bool TextClass::UpdateRenderPosition(ID3D11DeviceContext* deviceContext, int renderPositionX, int renderPositionY)
{
	// 해당 함수 재정의
	if (m_Font == nullptr) return false;
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

	int left = m_renderPositionX;
	int right = m_renderPositionX;
	int top = m_renderPositionY;
	int bottom = m_renderPositionY;

	int length = m_vertexCount / 6;

	for (int i = 0; i < length; i++)
	{
		CharType ct;
		m_Font->GetCharData(m_Str[i], &ct);

		if (ct.size == 0)
		{
			// 스페이스 바
			right += m_bitmapHeight/2;
		}
		else
		{
			// 왼쪽 위 기준 출력
			int width = (m_bitmapWidth / 2) * ct.size / 10;
			int height = (m_bitmapHeight / 2);

			right += width;
			bottom = m_renderPositionY - height;

			vertices[i * 6 + 0].position = XMFLOAT3(left, top, vertices[i * 6 + 0].position.z);
			vertices[i * 6 + 1].position = XMFLOAT3(right, top, vertices[i * 6 + 1].position.z);
			vertices[i * 6 + 2].position = XMFLOAT3(left, bottom, vertices[i * 6 + 2].position.z);
			vertices[i * 6 + 3].position = XMFLOAT3(left, bottom, vertices[i * 6 + 3].position.z);
			vertices[i * 6 + 4].position = XMFLOAT3(right, top, vertices[i * 6 + 4].position.z);
			vertices[i * 6 + 5].position = XMFLOAT3(right, bottom, vertices[i * 6 + 5].position.z);
		}

		left = right;
	}

	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void TextClass::ShutdownModel()
{
	BitmapClass::ShutdownModel();

	if (m_Str)
	{
		delete[] m_Str;
		m_Str = nullptr;
	}
}
