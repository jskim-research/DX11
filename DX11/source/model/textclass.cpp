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
	// Quad 하나만 있는 Vertex Buffer, Index Buffer
	// Size, Offset 등을 담는 Vertex Buffer, Index Buffer (Instance Data)
	bool result = false;
	ObjectParser::TextVertexInstanceType* instance_vertices = nullptr;
	ObjectParser::CommonVertexType* quad_vertices = nullptr;
	unsigned long* quad_indices = nullptr;

	// Instance Num = Strign Length
	m_StrLength = strlen(str);

	// 실제론 Quad 하나만 가지고 있음 (GPU Instancing 으로 GPU 한테 Quad 하나를 여러번 그리도록 명령할 것임)
	m_vertexCount = 4;
	m_indexCount = 6;

	instance_vertices = new ObjectParser::TextVertexInstanceType[m_StrLength];

	quad_vertices = new ObjectParser::CommonVertexType[4];
	quad_indices = new unsigned long[6];

	/*
	*	스크린의 중앙 원점 (0, 0) 을 LB 로 하는 하나의 Quad 구성 
	*/
	quad_vertices[0].position = XMFLOAT3(0, 1, 1);  // LT
	quad_vertices[1].position = XMFLOAT3(1, 1, 1);  // RT
	quad_vertices[2].position = XMFLOAT3(0, 0, 1);  // LB
	quad_vertices[3].position = XMFLOAT3(1, 0, 1);  // RB

	for (int i = 0; i < 4; i++)
	{
		quad_vertices[i].imageIndex = 0;
		quad_vertices[i].normal = XMFLOAT3(0, 0, -1);
		quad_vertices[i].color = color;
	}

	quad_indices[0] = 0;
	quad_indices[1] = 1;
	quad_indices[2] = 2;
	quad_indices[3] = 2;
	quad_indices[4] = 1;
	quad_indices[5] = 3;

	if (m_Str != nullptr) delete[] m_Str;
	m_Str = new char[m_StrLength];

	m_Font = &font;
	m_textureArrayView = font.GetTexture();

	// Instance = Quad = length
	for (int i = 0; i < m_StrLength; i++)
	{
		CharType ct;
		m_Str[i] = str[i];
		result = font.GetCharData(m_Str[i], &ct);
		if (!result) return false;

		// Quad uvRect = (left, top, right, bottom)
		instance_vertices[i].uvRect = XMFLOAT4(ct.u1, 0, ct.u2, 0.5);
		instance_vertices[i].offset = XMFLOAT2(0, 0);
		instance_vertices[i].size = XMFLOAT2(0, 0);
	}

	HRESULT hresult;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	// Quad Buffer 만들기 (CPU 에서 수정안한다는 것이 차이점)

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ObjectParser::CommonVertexType) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = quad_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_QuadVertexBuffer);
	if (FAILED(hresult))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = quad_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &m_QuadIndexBuffer);
	if (FAILED(hresult))
	{
		return false;
	}

	// Instance Buffer 만들기

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(ObjectParser::TextVertexInstanceType) * m_StrLength;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = instance_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_InstanceVertexBuffer);
	if (FAILED(hresult))
	{
		return false;
	}

	if (instance_vertices) delete[] instance_vertices;

	if (quad_vertices) delete[] quad_vertices;
	if (quad_indices) delete[] quad_indices;

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
	// CPU 에선 instance 별 offset, size 정도만 갱신하고 나머지 계산은 GPU 에 맡기는 형태
	ObjectParser::TextVertexInstanceType* vertices = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;

	result = deviceContext->Map(m_InstanceVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	vertices = (ObjectParser::TextVertexInstanceType*)mappedResource.pData;
	int deltaX = 0;

	for (int i = 0; i < m_StrLength; i++)
	{
		CharType ct;
		m_Font->GetCharData(m_Str[i], &ct);

		if (ct.size == 0)
		{
			// 스페이스바
			deltaX += (m_bitmapWidth / 2) * 5 / 10;
		}
		else
		{
			vertices[i].offset = XMFLOAT2(m_renderPositionX + deltaX, m_renderPositionY);
			vertices[i].size = XMFLOAT2((m_bitmapWidth / 2) * ct.size / 10, (m_bitmapHeight / 2));

			// x 축 이동만 고려
			deltaX += (m_bitmapWidth / 2) * ct.size / 10;
		}
	}

	deviceContext->Unmap(m_InstanceVertexBuffer, 0);

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

void TextClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	ID3D11Buffer* vertexBuffers[2] = { m_QuadVertexBuffer, m_InstanceVertexBuffer };
	UINT strides[2] = { sizeof(ObjectParser::CommonVertexType), sizeof(ObjectParser::TextVertexInstanceType) };
	UINT offsets[2] = { 0, 0 };


	// 하튼 다 GPU 한테 명령하는거임
	deviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
	deviceContext->IASetIndexBuffer(m_QuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 이 vertex buffer 로부터 render 될 primitive 형태 = triangle 임을 GPU 에게 알림
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
