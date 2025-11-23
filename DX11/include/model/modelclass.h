#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
using namespace DirectX;

#include "texture/textureclass.h"
#include "utility/objectparser.h"

// using CurVertexType = ObjectParser::... 맞춰줘야할듯

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

private:
	// 복사 금지
	ModelClass(const ModelClass&);

public:
	ModelClass();
	virtual ~ModelClass();

	virtual void Initialize(class D3DClass* direct3D);

	void Shutdown();
	/*
	*	Set Vertex Buffer & Index Buffer & PrimitiveTopology using model data
	*/
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetGltfTextures();

	bool ImportFromGLTF(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName);
	bool ImportFromCustomFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName);

protected:
	virtual void ShutdownModel();
	void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, const vector<const char*>&);
	void ReleaseTexture();

	bool MakeVertexBuffer(ID3D11Device* device, ObjectParser::CommonVertexType* vertices);
	bool MakeIndexBuffer(ID3D11Device* device, unsigned long* indices);

	D3D11_USAGE m_vertexBufferUsage;
	UINT m_vertexBufferCPUAccessFlags;
	D3D11_USAGE m_indexBufferUsage;
	UINT m_indexBufferCPUAccessFlags;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;

	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	ObjectParser* m_objectParser;
	ID3D11Texture2D* m_textureArray;
	ID3D11ShaderResourceView* m_textureArrayView;
	// Input layout desc 를 import 한 vertex buffer, index buffer 에 맞게 할당
	D3D11_INPUT_ELEMENT_DESC* m_polygonLayout;

};

#endif