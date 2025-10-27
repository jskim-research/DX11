#include "model/spriteclass.h"
#include <fstream>

SpriteClass::SpriteClass()
{
}

SpriteClass::~SpriteClass()
{
}

bool SpriteClass::ImportFromSpriteFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName)
{
	ObjectParser::CommonVertexType* vertices = 0;
	unsigned long* indices = 0;
	int vertexCount = 0;
	int indexCount = 0;
	bool result;

	result = m_objectParser->ParseCustomFile2(fileName, &vertices, &indices, &vertexCount, &indexCount);
	if (!result)
	{
		return false;
	}

	if (textureFileName)
	{
		ifstream fin;
		int textureCount = 0;
		vector<const char*> filenames;
		char input;

		fin.open(textureFileName);
		if (fin.fail()) return false;

		fin >> textureCount;
		fin.get(input);
		for (int i = 0; i < textureCount; i++)
		{
			char* filename = new char[128];

			int j = 0;
			fin.get(input);

			while (input != '\n')
			{
				filename[j++] = input;
				fin.get(input);
			}

			filename[j] = '\0';

			filenames.push_back(filename);
		}

		fin.close();

		if (!LoadTexture(device, deviceContext, filenames))
			return false;

		m_textureArrayView = m_Texture->GetTexture();
	}

	m_vertexCount = vertexCount;
	m_indexCount = indexCount;

	if (!MakeVertexBuffer(device, vertices))
	{
		return false;
	}

	if (!MakeIndexBuffer(device, indices))
	{
		return false;
	}

	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}

	if (indices)
	{
		delete[] indices;
		indices = 0;
	}

	return true;
}

bool SpriteClass::UpdateImage(ID3D11DeviceContext* deviceContext)
{
	/*
	*	Will be implemented soon.
	*/

	/*
	// 새로운 render position 에 따라 vertex buffer, index buffer 갱신
	ObjectParser::CommonVertexType* vertices = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;
	uint32_t imageIndex = 0;

	if (m_prevImageIndex == imageIndex)
	{
		return false;
	}

	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	m_prevImageIndex = imageIndex;

	vertices = (ObjectParser::CommonVertexType*)mappedResource.pData;
	vertices[0].imageIndex = imageIndex;
	vertices[1].imageIndex = imageIndex;
	vertices[2].imageIndex = imageIndex;
	vertices[3].imageIndex = imageIndex;
	vertices[4].imageIndex = imageIndex;
	vertices[5].imageIndex = imageIndex;

	deviceContext->Unmap(m_vertexBuffer, 0);
	*/

	return true;
}
