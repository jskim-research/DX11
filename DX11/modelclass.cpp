#include "modelclass.h"
#include "d3dclass.h"


ModelClass::ModelClass():
	m_vertexBufferUsage(D3D11_USAGE_DEFAULT),
	m_vertexBufferCPUAccessFlags(0),
	m_indexBufferUsage(D3D11_USAGE_DEFAULT),
	m_indexBufferCPUAccessFlags(0)
	
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_objectParser = 0;
	m_textureArray = 0;
	m_textureArrayView = 0;
}

ModelClass::~ModelClass()
{
}

void ModelClass::Initialize(D3DClass* direct3D)
{
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

ID3D11ShaderResourceView* ModelClass::GetGltfTextures()
{
	return m_textureArrayView;
}

bool ModelClass::ImportFromGLTF(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName)
{
	ObjectParser::CommonVertexType* gltf_vertices = 0;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	m_objectParser = new ObjectParser;

	std::vector<tinygltf::Image> images;
	bool parseResult = m_objectParser->ParseGLTFFile(fileName, &gltf_vertices, &indices, &m_vertexCount, &m_indexCount, images);

	if (!parseResult)
	{
		return false;
	}

	if (images.size() <= 0)
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// 모든 images 가 같은 resolution 이라고 가정
	textureDesc.Width = images[0].width;
	textureDesc.Height = images[0].height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = images.size();
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  // unsigned char *
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	result = device->CreateTexture2D(&textureDesc, 0, &m_textureArray);
	if (FAILED(result))
	{
		return false;
	}

	for (int i = 0; i < images.size(); i++)
	{
		tinygltf::Image& image = images[i];
		unsigned int rowPitch;
		rowPitch = (image.width * 4) * sizeof(unsigned char);

		deviceContext->UpdateSubresource(
			m_textureArray,
			D3D11CalcSubresource(0, i, textureDesc.MipLevels),  // Mip 0, Array Slice i, 해당 함수 mip level 수동 지정 필요
			NULL,
			image.image.data(),
			rowPitch,
			0);
	}

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = -1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = images.size();

	// Create the shader resource view for the texture.
	result = device->CreateShaderResourceView(m_textureArray, &srvDesc, &m_textureArrayView);
	if (FAILED(result))
	{

		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(m_textureArrayView);

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = m_vertexBufferUsage;
	vertexBufferDesc.ByteWidth = sizeof(ObjectParser::CommonVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = m_vertexBufferCPUAccessFlags;
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
	indexBufferDesc.Usage = m_indexBufferUsage;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = m_indexBufferCPUAccessFlags;
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

bool ModelClass::ImportFromCustomFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName)
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
		// 일단 기존 구현해둔 LoadTexture 를 쓰긴했는데
		// 깔끔하게 정리할 필요 있음
		if (!LoadTexture(device, deviceContext, textureFileName))
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
	// vertex buffer, index buffer 를 GPU 의 rendering pipeline stage 랑 연결하는 작업

	unsigned int stride;
	unsigned int offset;

	stride = sizeof(ObjectParser::CommonVertexType);
	offset = 0;

	// 하튼 다 GPU 한테 명령하는거임
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 이 vertex buffer 로부터 render 될 primitive 형태 = triangle 임을 GPU 에게 알림
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
{
	bool result;

	// Create and initialize the texture object.
	if (!m_Texture)
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

	if (m_textureArray)
	{
		m_textureArray->Release();
		m_textureArray = 0;
	}

	if (m_textureArrayView)
	{
		m_textureArrayView->Release();
		m_textureArrayView = 0;
	}
}

bool ModelClass::MakeVertexBuffer(ID3D11Device* device, ObjectParser::CommonVertexType* vertices)
{
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = m_vertexBufferUsage;
	vertexBufferDesc.ByteWidth = sizeof(ObjectParser::CommonVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = m_vertexBufferCPUAccessFlags;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool ModelClass::MakeIndexBuffer(ID3D11Device* device, unsigned long* indices)
{
	HRESULT result;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = m_indexBufferUsage;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = m_indexBufferCPUAccessFlags;
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

	return true;
}
