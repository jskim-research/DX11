#include "modelclass.h"
#include "objectparser.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_objectParser = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool result;

	result = InitializeBuffers(device, deviceContext);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

ID3D11ShaderResourceView** ModelClass::GetGltfTextures()
{
	return m_gltfTextureViews;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ObjectParser::GltfVertexType* gltf_vertices = 0;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	m_objectParser = new ObjectParser;

	std::vector<tinygltf::Image> images;
	const char* fileName = "./data/zhu_yuan.glb";
	bool parseResult = m_objectParser->ParseGLTFFile(fileName, &gltf_vertices, &indices, &m_vertexCount, &m_indexCount, images);

	if (!parseResult)
	{
		return false;
	}

	// GLTF 에 있는 texture 개수만큼 생성
	m_gltfTextures = new ID3D11Texture2D*[images.size()];
	m_gltfTextureViews = new ID3D11ShaderResourceView * [images.size()];

	for (int i = 0; i < images.size(); i++)
	{
		tinygltf::Image& image = images[i];
		HRESULT hResult;
		ID3D11Texture2D* tempTexture;
		ID3D11ShaderResourceView* tempTextureView;
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		unsigned int rowPitch;

		textureDesc.Width = image.width;
		textureDesc.Height = image.height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // unsigned char *
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = 0;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		result = device->CreateTexture2D(&textureDesc, 0, &tempTexture);
		if (FAILED(result))
		{
			return false;
		}

		// image.width * 4 는 RGBA 라는 뜻이겠지?
		rowPitch = (image.width * 4) * sizeof(unsigned char);
		// Copy the targa image data into the texture.
		deviceContext->UpdateSubresource(tempTexture, 0, NULL, image.image.data(), rowPitch, 0);
		// Setup the shader resource view description.
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create the shader resource view for the texture.
		hResult = device->CreateShaderResourceView(tempTexture, &srvDesc, &tempTextureView);
		if (FAILED(hResult))
		{
			return false;
		}

		// Generate mipmaps for this texture.
		deviceContext->GenerateMips(tempTextureView);
		m_gltfTextures[i] = tempTexture;
		m_gltfTextureViews[i] = tempTextureView;

	}

	// device 에 vertex, index buffer resource 생성
	// 생성 시 option 들을 설정하는 부분들 존재

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ObjectParser::GltfVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = gltf_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	if (gltf_vertices)
	{
		delete[] gltf_vertices;
		gltf_vertices = 0;
	}

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// vertex buffer, index buffer 를 GPU 에 active 하도록 하는 작업
	// 렌더 대상 데이터 세팅만 하는거임

	unsigned int stride;
	unsigned int offset;

	stride = sizeof(ObjectParser::GltfVertexType);
	offset = 0;

	// 하튼 다 GPU 한테 명령하는거임
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 이 vertex buffer 로부터 render 될 primitive 형태 = triangle 임을 GPU 에게 알림
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// Create and initialize the texture object.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}
