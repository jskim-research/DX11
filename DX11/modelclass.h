#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
using namespace DirectX;

#include "textureclass.h"

// using CurVertexType = ObjectParser::... ∏¬√Á¡‡æﬂ«“µÌ

class ObjectParser;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	ID3D11ShaderResourceView** GetGltfTextures();

private:
	bool InitializeBuffers(ID3D11Device*, ID3D11DeviceContext*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	ObjectParser* m_objectParser;

	// std::vector<ID3D11Texture2D*> m_gltfTextures;
	// std::vector<ID3D11ShaderResourceView*> m_gltfTextureViews;
	ID3D11Texture2D** m_gltfTextures;
	ID3D11ShaderResourceView** m_gltfTextureViews;
	int m_gltfTextureNum;
};

#endif